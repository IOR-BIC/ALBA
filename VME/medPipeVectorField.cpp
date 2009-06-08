/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeVectorField.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-06-08 15:11:33 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "medPipeVectorField.h"

#include "mafSceneNode.h"
#include "mafPipeFactory.h"
#include "mafGUI.h"
#include "mafGUIValidator.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutPreset.h"
#include "mafGUIPicButton.h"
#include "mafGUIMaterialChooser.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"

#include "vtkMAFAssembly.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkLookupTable.h"
#include "vtkGlyph3D.h"
//#include "vtkHedgeHog.h"
#include "vtkLineSource.h"
#include "vtkConeSource.h"
#include "vtkArrowSource.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkScalarBarActor.h"
#include "vtkRenderer.h"

#include "wx/busyinfo.h"
#include <float.h>


//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeVectorField);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeVectorField::medPipeVectorField() : mafPipe()
//----------------------------------------------------------------------------
{  
  m_VectorFieldIndex = 0;
  m_GlyphType = GLYPH_LINES;
  m_GlyphRadius = 0.1;
  m_GlyphLength = 1.0;
  m_GlyphRes = 6;
  
  m_GlyphScaling = SCALING_MAGNITUDE;
  m_ScalarFieldIndex = 0;
  
  m_GlyphMaterial = NULL;
  m_ColorMappingLUT = NULL;

  m_UseColorMapping = 1;
  m_UseSFColorMapping = 0;  
  m_ShowMap = 1;  
  
  m_GlyphLine = NULL;
  m_GlyphCone = NULL;
  m_GlyphArrow = NULL;

  m_Glyphs = NULL;
  m_GlyphsMapper = NULL;
  m_GlyphsActor = NULL;    
  
  m_SFActor = NULL; 
}

//----------------------------------------------------------------------------
medPipeVectorField::~medPipeVectorField()
//----------------------------------------------------------------------------
{  
  m_AssemblyFront->RemovePart(m_GlyphsActor);  
  m_RenFront->RemoveActor2D(m_SFActor);  
  
  vtkDEL(m_SFActor);
  
  vtkDEL(m_GlyphsActor);
  vtkDEL(m_GlyphsMapper);
  vtkDEL(m_Glyphs);
  vtkDEL(m_GlyphArrow);
  vtkDEL(m_GlyphCone);
  vtkDEL(m_GlyphLine);
  
  vtkDEL(m_ColorMappingLUT);
  mafDEL(m_GlyphMaterial);    
}


//----------------------------------------------------------------------------
void medPipeVectorField::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);  
 
  //calculate parameters derived from the input (such as radius of tube and
  //particles, or sampling coefficient)
  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  ds->Update(); //force its update

  double dblDiag = ds->GetLength();
  m_GlyphRadius = dblDiag / 80;
  
  if (GetNumberOfVectors() > 0)
  {
    //detect the default one vector field
    if ((m_VectorFieldIndex = GetVectorFieldIndex("velocity")) < 0)          
        m_VectorFieldIndex = 0; //no default vector field, use the first one    

    if ((m_ScalarFieldIndex = GetScalarFieldIndex()) < 0)          
      m_ScalarFieldIndex = 0; //no default vector field, use the first one       

    //now let us create VTK pipe
    CreateVTKPipe();  
    UpdateVTKPipe();  //and set visual properties as needed  
  }
}

//----------------------------------------------------------------------------
mafGUI *medPipeVectorField::CreateGui()
//----------------------------------------------------------------------------
{
  int nVectors = GetNumberOfVectors();
  int nScalars = GetNumberOfScalars();

  m_Gui = new mafGUI(this);
     
  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );
  if (nVectors == 0)  //no vector field available, this pipe does not work
    bSizerMain->Add(new wxStaticText( m_Gui, wxID_ANY, _("No vector field to visualize.")));
  else
  {
    if (nVectors > 1)      
    {
      wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
      bSizer1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Vector Field:"), 
        wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboField = new wxComboBox( m_Gui, ID_VECTORFIELD, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboField->SetToolTip( 
        _("Selects the vector field for which streamlines should be computed.") );
      PopulateCombo(comboField, true);  //at least one vector is available
      bSizer1->Add( comboField, 1, wxALL, 1 );
      bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );

      //and validator
      comboField->SetValidator(mafGUIValidator(this, ID_VECTORFIELD, comboField, &m_VectorFieldIndex));
    }

    if (nScalars > 1)
    {
      wxBoxSizer* bSizer51111 = new wxBoxSizer( wxHORIZONTAL );  
      bSizer51111->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scalar Field:"), 
        wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboScalars = new wxComboBox( m_Gui, ID_SCALARFIELD, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboScalars->SetToolTip( 
        _("Selects the scalar field to be used for scaling and coloring of glyphs.") );
      PopulateCombo(comboScalars, false);
      bSizer51111->Add( comboScalars, 1, wxALL, 1 );
      bSizerMain->Add( bSizer51111, 0, wxEXPAND, 5 );

      //and validator
      comboScalars->SetValidator(mafGUIValidator(this, ID_SCALARFIELD, comboScalars, &m_ScalarFieldIndex));
    }

    wxBoxSizer* bSizer52 = new wxBoxSizer( wxHORIZONTAL );
    bSizer52->Add( new wxStaticText( m_Gui, wxID_ANY, _("Glyph Type:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxComboBox* comboGlyphs = new wxComboBox( m_Gui, ID_GLYPH_TYPE, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    comboGlyphs->Append( _("lines") );
    comboGlyphs->Append( _("cones") );
    comboGlyphs->Append( _("arrows") );
    comboGlyphs->SetToolTip( _("Specifies glyph type which should be used "
      "for the visualization of the selected vector field.") );
    bSizer52->Add( comboGlyphs, 1, wxALL, 0 );
    bSizerMain->Add( bSizer52, 0, wxEXPAND, 5 );

    //and validator
    comboGlyphs->SetValidator(mafGUIValidator(this, ID_GLYPH_TYPE, comboGlyphs, &m_GlyphType));

#pragma region Glyph shape
    wxStaticBoxSizer* sbSizer2 = new wxStaticBoxSizer( 
      new wxStaticBox( m_Gui, wxID_ANY, _("Glyph Shape") ), wxVERTICAL );
    wxBoxSizer* bSizer5 = new wxBoxSizer( wxHORIZONTAL );
    bSizer5->Add( new wxStaticText( m_Gui, wxID_ANY, _("Radius:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxTextCtrl* edRadius = new wxTextCtrl( m_Gui, ID_GLYPH_RADIUS);
    edRadius->SetToolTip( _("Specifies the radius of glyph, i.e., "
      "the thickness of lines, radius of cones and arrows. ") );
    bSizer5->Add( edRadius, 1, wxALL, 1 );

    //and validator
    edRadius->SetValidator(mafGUIValidator(this, ID_GLYPH_RADIUS, edRadius, &m_GlyphRadius, 1e-8, 1000));

    bSizer5->Add( new wxStaticText( m_Gui, wxID_ANY, _("Res:"), 
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT ), 1, wxALL, 5 );

    wxTextCtrl* edRes = new wxTextCtrl( m_Gui, ID_GLYPH_RESOLUTION);
    edRes->SetToolTip( _("Specifies the resolution of glyphs, "
      "i.e., number of sides of cones, etc.") );
    bSizer5->Add( edRes, 1, wxALL, 1 );
    sbSizer2->Add( bSizer5, 0, wxEXPAND, 5 );

    //and validator
    edRes->SetValidator(mafGUIValidator(this, ID_GLYPH_RESOLUTION, edRes, &m_GlyphRes, 3, 100));

    wxBoxSizer* bSizer51 = new wxBoxSizer( wxHORIZONTAL );
    bSizer51->Add( new wxStaticText( m_Gui, wxID_ANY, _("Length:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxTextCtrl* edLength = new wxTextCtrl( m_Gui, ID_GLYPH_LENGTH);
    edLength->SetToolTip( _("Specifies the length of glyph, which will "
      "be scaled by the selected scaling component.") );
    bSizer51->Add( edLength, 1, wxALL, 1 );

    //and validator
    edLength->SetValidator(mafGUIValidator(this, ID_GLYPH_LENGTH, edLength, &m_GlyphLength, 1e-8, 1000));    

    bSizer51->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scaling:"), 
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT ), 1, wxALL, 5 );

    wxComboBox* comboScale = new wxComboBox( m_Gui, ID_GLYPH_SCALING, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );  
    comboScale->Append( _("off") );
    comboScale->Append( _("magnitude") );
    if (nScalars != 0)
      comboScale->Append( _("scalar") );    
    comboScale->SetToolTip( _("Specifies how the length of glyph is scaled, "
      "whether by the magnitude of vectors, associated scalar value (from the "
      "selected scalar field) or not scaled at all.") );
    bSizer51->Add( comboScale, 1, wxALL, 0 );
    sbSizer2->Add( bSizer51, 1, wxEXPAND, 0 );

    //and validator
    comboScale->SetValidator(mafGUIValidator(this, ID_GLYPH_SCALING, comboScale, &m_GlyphScaling));

    bSizerMain->Add( sbSizer2, 0, wxEXPAND, 5 );
#pragma endregion Glyph shape

#pragma region Glyph Colors
    wxStaticBoxSizer* sbSizer65 = new wxStaticBoxSizer( 
      new wxStaticBox( m_Gui, wxID_ANY, _("Glyph Color") ), wxVERTICAL );

    wxBoxSizer* bSizer4311 = new wxBoxSizer( wxHORIZONTAL );
    bSizer4311->Add( new wxStaticText( m_Gui, wxID_ANY, _("Material:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_GlyphMaterial->GetMaterial()->MakeIcon();
    m_GlyphMaterialButton = new mafGUIPicButton(m_Gui, 
      m_GlyphMaterial->GetMaterial()->m_Icon, ID_GLYPH_MATERIAL, this);
    bSizer4311->Add( m_GlyphMaterialButton, 0, wxALL, 0 );

    m_GlyphMaterialLabel = new wxStaticText( m_Gui, ID_GLYPH_MATERIAL_LABEL, 
      m_GlyphMaterial->GetMaterial()->m_MaterialName.GetCStr(), wxDefaultPosition, wxDefaultSize, 0 );        
    bSizer4311->Add( m_GlyphMaterialLabel, 1, wxALL, 5 );
    sbSizer65->Add( bSizer4311, 0, wxEXPAND, 5 );

    wxCheckBox* chckShowScalarField1 = new wxCheckBox( m_Gui, 
      ID_USE_COLOR_MAPPING, _("Enable Color Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
    chckShowScalarField1->SetToolTip( 
      _("If checked, glyphs are colored using a mapping of the specified LUT.") );
    sbSizer65->Add( chckShowScalarField1, 0, wxALL|wxEXPAND, 5 );

    //and validator
    chckShowScalarField1->SetValidator(mafGUIValidator(this, ID_USE_COLOR_MAPPING, 
      chckShowScalarField1, &m_UseColorMapping));
    

    wxStaticBoxSizer* sbSizer221 = new wxStaticBoxSizer( 
      new wxStaticBox( m_Gui, wxID_ANY, wxEmptyString ), wxVERTICAL );

    //scalar field widgets
    if (nScalars > 0)
    {      
      wxCheckBox* checkBox52 = new wxCheckBox( m_Gui, ID_USE_SF_MAPPING, 
         _("Scalar Field Mapping"), wxDefaultPosition, wxDefaultSize, 0 );    
      checkBox52->SetToolTip( _("If checked, values from the specified scalar "
        "field are used to map colors, otherwise vector magnitudes are used.") );
      sbSizer221->Add( checkBox52, 0, wxALL, 5 );
      
      //and validator
      checkBox52->SetValidator(mafGUIValidator(this, ID_USE_SF_MAPPING,       
        checkBox52, &m_UseSFColorMapping));     
    }

    wxBoxSizer* bSizer491 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* bSizer432 = new wxBoxSizer( wxHORIZONTAL );    
    bSizer432->Add( new wxStaticText( m_Gui, wxID_ANY, _("LUT:")), 0, wxALL, 5 );

    mafGUILutSwatch* luts2 = new mafGUILutSwatch(m_Gui, ID_SF_MAPPING_LUT, 
      wxDefaultPosition, wxSize(140, 18), wxTAB_TRAVERSAL | wxSIMPLE_BORDER );
    luts2->SetLut(m_ColorMappingLUT);
    luts2->SetEditable(true);
    luts2->SetListener(this);
    bSizer432->Add(luts2, 0, 0/*wxEXPAND*/, 5 );
    bSizer491->Add( bSizer432, 1, wxEXPAND, 5 );

    wxCheckBox* chckShowSFLegend1 = new wxCheckBox( m_Gui, ID_SHOW_SF_MAPPING, 
      _("Show Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
    chckShowSFLegend1->SetToolTip( _("If checked, mapping is displayed "
      "as a colored bar in the main view.") );
    bSizer491->Add( chckShowSFLegend1, 0, wxALL|wxEXPAND, 5 );

    //and validator
    chckShowSFLegend1->SetValidator(mafGUIValidator(this, ID_SHOW_SF_MAPPING,       
      chckShowSFLegend1, &m_ShowMap));

    sbSizer221->Add( bSizer491, 1, wxEXPAND, 5 );
    sbSizer65->Add( sbSizer221, 0, wxEXPAND, 5 );
    bSizerMain->Add( sbSizer65, 0, wxEXPAND, 0 );
#pragma endregion Glyph Colors
  }

  m_Gui->Add(bSizerMain);
  return m_Gui;
}


//----------------------------------------------------------------------------
void medPipeVectorField::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{			
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
      if (e->GetId() == ID_GLYPH_MATERIAL)
        OnChangeMaterial();
        
      UpdateVTKPipe(); 
    
      mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      return;
    }     
  }

  //forward it to our listener to deal with it
  mafEventMacro(*maf_event);
}

//------------------------------------------------------------------------
// Handles change of material.
/*virtual*/ void medPipeVectorField::OnChangeMaterial()
//------------------------------------------------------------------------
{    
  mafGUIMaterialChooser dlg;
  if (dlg.ShowChooserDialog(m_GlyphMaterial))
  {
    //set new material label
    mmaMaterial* mat = m_GlyphMaterial->GetMaterial();     
    m_GlyphMaterialLabel->SetLabel(mat->m_MaterialName.GetCStr());

    //and set a new material icon
    cppDEL(mat->m_Icon); mat->MakeIcon();
    m_GlyphMaterialButton->SetBitmapLabel(*mat->m_Icon);
    m_GlyphMaterialButton->Refresh();

    m_Gui->Update();
  }
}

//------------------------------------------------------------------------
//Returns the index of specified field (scalar or vectors depending on
//bVectors parameter). If szName is NULL, the index of currently active
//(scalar or vector) field is returned.
//The routine returns -1, if it cannot find appropriate field.
int medPipeVectorField::GetFieldIndex(const char* szName, bool bVectors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return -1;

  int nQueryComps;
  vtkDataArray* pQueryDA;

  if (!bVectors)
  {
    nQueryComps = 1;
    pQueryDA = pd->GetScalars(szName);
  }
  else
  {
    nQueryComps = 3;
    pQueryDA = pd->GetVectors(szName);
  }
   
  if (pQueryDA == NULL)
    return -1;

  int nIndex = 0;
  int nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    if (da == pQueryDA)
      break;  //we found it
    
    if (da->GetNumberOfComponents() == nQueryComps)
      nIndex++; //this is correct array
  }
  
  return nIndex;
}

//------------------------------------------------------------------------
//Returns the number of available scalars/vectors.
int medPipeVectorField::GetNumberOfFields(bool bVectors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return -1;

  int nQueryComps = bVectors ? 3 : 1;
  int nRet = 0, nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    if (da->GetNumberOfComponents() == nQueryComps)
      nRet++; //this is correct array
  }

  return nRet;
}

//------------------------------------------------------------------------
//Returns the name of field (scalar or vectors depending on
//bVectors parameter) at the specified index. 
//The routine returns NULL, if it cannot find appropriate field.
const char* medPipeVectorField::GetFieldName(int nIndex, bool bVectors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd == NULL)
    return NULL;

  int nQueryComps = bVectors ? 3 : 1;
  int nCount = pd->GetNumberOfArrays();
  for (int i = 0; i < nCount; i++)
  {
    vtkDataArray* da = pd->GetArray(i);
    if (da->GetNumberOfComponents() == nQueryComps)
    {
      if (nIndex == 0)
        return da->GetName();
      
      nIndex--;
    }
  }

  return NULL;
}

//------------------------------------------------------------------------
//Populates the combo box by names of scalar/vector fields
void medPipeVectorField::PopulateCombo(wxComboBox* combo, bool bVectors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  if (pd != NULL)
  {
    int nQueryComps = bVectors ? 3 : 1;
    int nCount = pd->GetNumberOfArrays();
    for (int i = 0; i < nCount; i++)
    {
      vtkDataArray* da = pd->GetArray(i);
      if (da->GetNumberOfComponents() == nQueryComps){
        combo->Append(da->GetName());
      }
    }
  }
}

//------------------------------------------------------------------------
//Constructs VTK pipeline.
/*virtual*/ void medPipeVectorField::CreateVTKPipe()
//------------------------------------------------------------------------
{
  //build materials  
  mafNEW(m_GlyphMaterial);

  //build LUT
  m_ColorMappingLUT = vtkLookupTable::New();
  lutPreset(12, m_ColorMappingLUT); //initialize LUT to SAR (it has index 12)  
  m_ColorMappingLUT->Build();

  m_GlyphLine = vtkLineSource::New();   
  m_GlyphCone = vtkConeSource::New(); 
  m_GlyphArrow = vtkArrowSource::New(); 

  m_GlyphLine->SetPoint1(0.0, 0.0, 0.0);
  m_GlyphLine->SetPoint2(1.0, 0.0, 0.0);
//  m_GlyphLine->CappingOn();
  m_GlyphCone->CappingOn();  
  m_GlyphArrow->SetTipLength(0.5);     

  m_Glyphs = vtkGlyph3D::New();
  m_Glyphs->SetInput(m_Vme->GetOutput()->GetVTKData());  
  m_Glyphs->SetVectorModeToUseVector();

  m_GlyphsMapper = vtkPolyDataMapper::New();
  m_GlyphsMapper->SetInput(m_Glyphs->GetOutput());
  m_GlyphsMapper->ImmediateModeRenderingOn();
  m_GlyphsMapper->SetScalarModeToUsePointData();
  m_GlyphsMapper->SetColorModeToMapScalars();
  m_GlyphsMapper->SetLookupTable(m_ColorMappingLUT);

  m_GlyphsActor = vtkActor::New();
  m_GlyphsActor->SetMapper(m_GlyphsMapper);
  m_GlyphsActor->SetPickable(0);   //make it faster

  //scalar field map
  m_SFActor = vtkScalarBarActor::New();
  m_SFActor->SetLookupTable(m_GlyphsMapper->GetLookupTable());
  ((vtkActor2D*)m_SFActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  ((vtkActor2D*)m_SFActor)->GetPositionCoordinate()->SetValue(0.1,0.01);
  m_SFActor->SetOrientationToHorizontal();
  m_SFActor->SetWidth(0.8);
  m_SFActor->SetHeight(0.12);  
  m_SFActor->SetLabelFormat("%6.3g");
  m_SFActor->SetPickable(0);   //make it faster

  m_RenFront->AddActor2D(m_SFActor);
  m_AssemblyFront->AddPart(m_GlyphsActor); 
}

//------------------------------------------------------------------------
//Updates VTK pipeline (setting radius, etc.). 
/*virtual*/ void medPipeVectorField::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  m_Glyphs->SelectInputVectors(GetVectorFieldName(m_VectorFieldIndex));

  if (m_GlyphType == GLYPH_LINES)
  {
    //m_GlyphLine->SetRadius(m_GlyphRadius);
    //m_GlyphLine->SetHeight(m_GlyphLength);
    //m_GlyphLine->SetResolution(m_GlyphRes);
    //
    m_Glyphs->SetSource(m_GlyphLine->GetOutput());
  }
  else if (m_GlyphType == GLYPH_CONES)
  {
    m_GlyphCone->SetRadius(m_GlyphRadius);
    m_GlyphCone->SetHeight(m_GlyphLength);
    m_GlyphCone->SetResolution(m_GlyphRes);

    m_Glyphs->SetSource(m_GlyphCone->GetOutput());
  }
  else
  {
    m_GlyphArrow->SetShaftRadius(0.3 * m_GlyphRadius);    
    m_GlyphArrow->SetShaftResolution(m_GlyphRes);

    m_GlyphArrow->SetTipRadius(m_GlyphRadius);    
    m_GlyphArrow->SetTipResolution(m_GlyphRes);
    
    m_Glyphs->SetSource(m_GlyphArrow->GetOutput());
  }

  if (m_GlyphScaling == SCALING_OFF)
    m_Glyphs->SetScaleModeToDataScalingOff();
  else if (m_GlyphScaling == SCALING_MAGNITUDE)
    m_Glyphs->SetScaleModeToScaleByVector();
  else
    m_Glyphs->SetScaleModeToScaleByScalar();

  m_Glyphs->SelectInputScalars(GetScalarFieldName(m_ScalarFieldIndex));
  
  if (m_UseSFColorMapping == 0)
    m_Glyphs->SetColorModeToColorByVector();
  else
    m_Glyphs->SetColorModeToColorByScalar();
    

  m_Glyphs->Update();
    
  if (m_UseColorMapping != 0)
  {
    vtkDataArray* da = m_Glyphs->GetOutput()->GetPointData()->GetScalars();
    if (da != NULL)
    {
      double sr[2];
      da->GetRange(sr);
      m_ColorMappingLUT->SetTableRange(sr);

      m_GlyphsMapper->SetScalarRange(sr);        
    }
  }

  m_GlyphsMapper->SetScalarVisibility(m_UseColorMapping); 
  m_GlyphsMapper->Update();

  m_GlyphsActor->SetProperty(m_GlyphMaterial->GetMaterial()->m_Prop);
  m_GlyphsActor->Modified();
//  m_GlyphsActor->SetVisibility();  

//  m_SFActor->SetTitle(scalar_name);
  m_SFActor->SetVisibility(m_ShowMap != 0 && m_UseColorMapping != 0);  
}