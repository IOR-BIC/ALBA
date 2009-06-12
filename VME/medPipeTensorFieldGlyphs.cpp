/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeTensorFieldGlyphs.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-06-12 16:34:48 $ 
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
#include "medPipeTensorFieldGlyphs.h"

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
#include "vtkTensorGlyph.h"

#include "vtkSphereSource.h"
#include "vtkArrowSource.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkScalarBarActor.h"
#include "vtkRenderer.h"

#include "wx/busyinfo.h"
#include <float.h>
#include <math.h>

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeTensorFieldGlyphs);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeTensorFieldGlyphs::medPipeTensorFieldGlyphs() : medPipeTensorField()
//----------------------------------------------------------------------------
{    
  m_GlyphType = GLYPH_ELLIPSOID;
  m_GlyphRadius = 0.1;  
  m_GlyphRes = 3;
  
  m_GlyphScaling = 1;  
  
  m_GlyphMaterial = NULL;
  m_ColorMappingLUT = NULL;

  m_UseColorMapping = 1;
  m_UseSFColorMapping = 0;  
  m_ShowMap = 1;  
  
  m_GlyphEllipsoid = NULL;  
  m_GlyphArrow = NULL;

  m_Glyphs = NULL;
  m_GlyphsMapper = NULL;
  m_GlyphsActor = NULL;    
  
  m_SFActor = NULL; 
}

//----------------------------------------------------------------------------
medPipeTensorFieldGlyphs::~medPipeTensorFieldGlyphs()
//----------------------------------------------------------------------------
{  
  m_AssemblyFront->RemovePart(m_GlyphsActor);  
  m_RenFront->RemoveActor2D(m_SFActor);  
  
  vtkDEL(m_SFActor);
  
  vtkDEL(m_GlyphsActor);
  vtkDEL(m_GlyphsMapper);
  vtkDEL(m_Glyphs);
  vtkDEL(m_GlyphArrow);
  vtkDEL(m_GlyphEllipsoid);  
  
  vtkDEL(m_ColorMappingLUT);
  mafDEL(m_GlyphMaterial);    
}


//----------------------------------------------------------------------------
//Default radius, etc. should be calculated in this method, 
//i.e., inherited classes should always override this method. 
//The default implementation is to update VME
/*virtual*/ void medPipeTensorFieldGlyphs::ComputeDefaultParameters()
//----------------------------------------------------------------------------
{
  Superclass::ComputeDefaultParameters();

  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  double dblDiag = ds->GetLength();
  double nPoints = (double)ds->GetNumberOfPoints();
      
  m_GlyphRadius = dblDiag / (2*pow(nPoints, 1 / 3.0));  
  //should be scaled by 100 for some data
}

//----------------------------------------------------------------------------
mafGUI *medPipeTensorFieldGlyphs::CreateGui()
//----------------------------------------------------------------------------
{
  int nTensors = GetNumberOfTensors();
  int nScalars = GetNumberOfScalars();

  m_Gui = new mafGUI(this);
     
  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );
  if (nTensors == 0)  //no tensor field available, this pipe does not work
    bSizerMain->Add(new wxStaticText( m_Gui, wxID_ANY, _("No tensor field to visualize.")));
  else
  {
    if (nTensors > 1)      
    {
      wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
      bSizer1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Tensor Field:"), 
        wxDefaultPosition, wxSize( 65,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboField = new wxComboBox( m_Gui, ID_TENSORFIELD, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboField->SetToolTip(_("Selects the tensor field to be visualized.") );
      PopulateCombo(comboField, true);  //at least one tensor is available
      bSizer1->Add( comboField, 1, wxALL, 1 );
      bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );

      //and validator
      comboField->SetValidator(mafGUIValidator(this, ID_TENSORFIELD, comboField, &m_TensorFieldIndex));
    }

    if (nScalars > 1)
    {
      wxBoxSizer* bSizer51111 = new wxBoxSizer( wxHORIZONTAL );  
      bSizer51111->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scalar Field:"), 
        wxDefaultPosition, wxSize( 65,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboScalars = new wxComboBox( m_Gui, ID_SCALARFIELD, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboScalars->SetToolTip( 
        _("Selects the scalar field to be used for coloring of glyphs.") );
      PopulateCombo(comboScalars, false);
      bSizer51111->Add( comboScalars, 1, wxALL, 1 );
      bSizerMain->Add( bSizer51111, 0, wxEXPAND, 5 );

      //and validator
      comboScalars->SetValidator(mafGUIValidator(this, ID_SCALARFIELD, comboScalars, &m_ScalarFieldIndex));
    }

    wxBoxSizer* bSizer52 = new wxBoxSizer( wxHORIZONTAL );
    bSizer52->Add( new wxStaticText( m_Gui, wxID_ANY, _("Glyph Type:"), 
      wxDefaultPosition, wxSize( 65,-1 ), 0 ), 0, wxALL, 5 );

    wxComboBox* comboGlyphs = new wxComboBox( m_Gui, ID_GLYPH_TYPE, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    comboGlyphs->Append( _("ellipsoid") );
    comboGlyphs->Append( _("arrows") );
    comboGlyphs->SetToolTip( _("Specifies glyph type which should be used "
      "for the visualization of the selected tensor field.") );
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
    edRadius->SetToolTip( _("Specifies the unscaled size of glyph. ") );
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

    //BES: 12.6.2009 - vtkTensorGlyph version 4.2 contains "Eigen Values Scaling"
    //but does not implement it => we won't use it
    //wxCheckBox* chckScale = new wxCheckBox( m_Gui, ID_GLYPH_SCALING, 
    //  _("Enable Eigen Values Scaling"), wxDefaultPosition, wxDefaultSize, 0 );
    //chckScale->SetToolTip( _("When checked, glyphs are scaled according "
    //  "to the eigen values of tensors.") );
    //sbSizer2->Add( chckScale, 0, wxALL, 5 );

    ////and validator
    //chckScale->SetValidator(mafGUIValidator(this, ID_GLYPH_SCALING, chckScale, &m_GlyphScaling));

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
        "field are used to map colors, otherwise eigen values are used.") );
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
void medPipeTensorFieldGlyphs::OnEvent(mafEventBase *maf_event)
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
/*virtual*/ void medPipeTensorFieldGlyphs::OnChangeMaterial()
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
//Constructs VTK pipeline.
/*virtual*/ void medPipeTensorFieldGlyphs::CreateVTKPipe()
//------------------------------------------------------------------------
{
  //build materials  
  mafNEW(m_GlyphMaterial);

  //build LUT
  m_ColorMappingLUT = vtkLookupTable::New();
  lutPreset(12, m_ColorMappingLUT); //initialize LUT to SAR (it has index 12)  
  m_ColorMappingLUT->Build();
  
  m_GlyphEllipsoid = vtkSphereSource::New(); 
  m_GlyphArrow = vtkArrowSource::New(); 
   
  m_GlyphArrow->SetTipLength(0.5);     

  m_Glyphs = vtkTensorGlyph::New();
  m_Glyphs->SetInput(m_Vme->GetOutput()->GetVTKData());  
  m_Glyphs->SetScaleFactor(1.0);
  m_Glyphs->ClampScalingOff();
  m_Glyphs->SymmetricOff();  

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
/*virtual*/ void medPipeTensorFieldGlyphs::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  m_Vme->GetOutput()->GetVTKData()->GetPointData()->
    SetActiveTensors(GetTensorFieldName(m_TensorFieldIndex));

  m_Vme->GetOutput()->GetVTKData()->GetPointData()->
    SetActiveScalars(GetScalarFieldName(m_ScalarFieldIndex));

  if (m_GlyphType == GLYPH_ELLIPSOID)
  {
//    m_GlyphEllipsoid->SetRadius(m_GlyphRadius);  
    m_GlyphEllipsoid->SetPhiResolution(m_GlyphRes);
    m_GlyphEllipsoid->SetThetaResolution(m_GlyphRes);

    m_Glyphs->SetSource(m_GlyphEllipsoid->GetOutput());
    m_Glyphs->ThreeGlyphsOff();
  }  
  else
  {
    m_GlyphArrow->SetShaftRadius(0.3 * m_GlyphRadius);    
    m_GlyphArrow->SetShaftResolution(m_GlyphRes);

    m_GlyphArrow->SetTipRadius(m_GlyphRadius);    
    m_GlyphArrow->SetTipResolution(m_GlyphRes);
    
    m_Glyphs->SetSource(m_GlyphArrow->GetOutput());
    m_Glyphs->ThreeGlyphsOn();
  }
  
  m_Glyphs->SetScaleFactor(m_GlyphRadius);
  m_Glyphs->SetScaling(m_GlyphScaling); 
  
  if (m_UseSFColorMapping == 0)
    m_Glyphs->SetColorModeToEigenvalues();
  else
    m_Glyphs->SetColorModeToScalars();
    
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

  m_SFActor->SetVisibility(m_ShowMap != 0 && m_UseColorMapping != 0);  
}