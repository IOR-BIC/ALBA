/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVectorFieldMapWithArrows.cpp,v $
  Language:  C++
  Date:      $Date: 2010-12-09 16:43:02 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Simone Brazzale
==========================================================================
  Copyright (c) 2001/2005
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "medPipeVectorFieldMapWithArrows.h"

#include "mafSceneNode.h"
#include "mafPipeFactory.h"
#include "mafGUI.h"
#include "mafGUIValidator.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutPreset.h"
#include "mafGUIFloatSlider.h"
#include "mafVME.h"

#include "vtkArrowSource.h"
#include "vtkConeSource.h"
#include "vtkGlyph3D.h"

#include "vtkMAFAssembly.h"
#include "vtkDataSet.h"
#include "vtkGeometryFilter.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkLookupTable.h"

#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataMapper.h"
#include "vtkScalarBarActor.h"
#include "vtkGeometryFilter.h"

#include "wx/busyinfo.h"
#include <float.h>

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeVectorFieldMapWithArrows);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeVectorFieldMapWithArrows::medPipeVectorFieldMapWithArrows() : medPipeVectorField()
//----------------------------------------------------------------------------
{     
  m_ColorMappingLUT = NULL;  
  m_ShowMapping = 1;  
  m_ShowSurface = 1;
  m_ShowGlyphs = 1;
  m_ScalingValue = 1;
    
  m_MappingActor = NULL;
  m_ScalarColorMappingMode = 0;
  m_VectorColorMappingMode = 0;  
  m_ActivateScalars = 0;
  m_ActivateVectors = 1;
    
  m_SurfaceActor = NULL;
  m_SurfaceMapper = NULL;

  m_Glyph = NULL;
  m_GlyphMapper = NULL;
  m_GlyphActor = NULL;

  m_ComboField_s = NULL;
  m_ComboField_v = NULL;
  m_ComboColorBy_s = NULL;
  m_ComboColorBy_v = NULL;
}

//----------------------------------------------------------------------------
medPipeVectorFieldMapWithArrows::~medPipeVectorFieldMapWithArrows()
//----------------------------------------------------------------------------
{ 
  m_AssemblyFront->RemovePart(m_GlyphActor); 
  m_AssemblyFront->RemovePart(m_SurfaceActor);
  m_RenFront->RemoveActor2D(m_MappingActor);  
  
  vtkDEL(m_MappingActor);
  
  vtkDEL(m_Glyph);
  vtkDEL(m_GlyphActor);
  vtkDEL(m_GlyphMapper);
  
  vtkDEL(m_SurfaceActor);
  vtkDEL(m_SurfaceMapper);
  
  vtkDEL(m_ColorMappingLUT); 

  cppDEL(m_ComboField_s);
  cppDEL(m_ComboField_v);
  cppDEL(m_ComboColorBy_s);
  cppDEL(m_ComboColorBy_v);
}


//----------------------------------------------------------------------------
mafGUI *medPipeVectorFieldMapWithArrows::CreateGui()
//----------------------------------------------------------------------------
{  
  int nVectors = GetNumberOfVectors();
  int nScalars = GetNumberOfScalars();
  
  m_Gui = new mafGUI(this);

  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );
     
  m_Gui->Divider();
  m_Gui->Label("Vector field mapping properties:", false);
  m_Gui->Divider();

  if (nScalars>0)
  {
    wxCheckBox* ckScalars = new wxCheckBox( m_Gui, ID_ACTIVATE_SCALARS, _("Activate scalar fields"), wxDefaultPosition, wxDefaultSize, 0 );
    ckScalars->SetToolTip( _("If checked, scalar fields are displayed.") );
    bSizerMain->Add( ckScalars, 0, wxALL|wxEXPAND, 5 );

    //add validator
    ckScalars->SetValidator(mafGUIValidator(this, ID_ACTIVATE_SCALARS, ckScalars, &m_ActivateScalars));
    if (nScalars > 1)      
    {
      wxBoxSizer* bSizer_s1 = new wxBoxSizer( wxHORIZONTAL );
      bSizer_s1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scalar Field:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

      m_ComboField_s = new wxComboBox( m_Gui, ID_SCALARFIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      m_ComboField_s->SetToolTip( _("Selects the scalar field to be visualized.") );
      PopulateCombo(m_ComboField_s, false);  //at least one scalar is available
      bSizer_s1->Add( m_ComboField_s, 1, wxALL, 1 );
      bSizerMain->Add( bSizer_s1, 0, wxEXPAND, 0 );

      //and validator
      m_ComboField_s->SetValidator(mafGUIValidator(this, ID_SCALARFIELD, m_ComboField_s, &m_ScalarFieldIndex));
      m_ComboField_s->Enable(false);
    }
    
    wxBoxSizer* bSizer_s2 = new wxBoxSizer( wxHORIZONTAL );
    bSizer_s2->Add( new wxStaticText( m_Gui, wxID_ANY, _("Map by:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_ComboColorBy_s = new wxComboBox( m_Gui, ID_SCALAR_COLOR_MAPPING_MODE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    m_ComboColorBy_s->Append( _("Scalar value") );
    m_ComboColorBy_s->SetToolTip(_("Specifies how the specified scalar field should be mapped on to the surface.") );
    bSizer_s2->Add( m_ComboColorBy_s, 1, wxALL, 0 );
    bSizerMain->Add( bSizer_s2, 0, wxEXPAND, 5 );

    //and validator
    m_ComboColorBy_s->SetValidator(mafGUIValidator(this, ID_SCALAR_COLOR_MAPPING_MODE, m_ComboColorBy_s, &m_ScalarColorMappingMode));
    m_ComboColorBy_s->Enable(false);
  }
  if (nVectors>0)
  {
    wxCheckBox* ckVectors = new wxCheckBox( m_Gui, ID_ACTIVATE_VECTORS, _("Activate vector fields"), wxDefaultPosition, wxDefaultSize, 0 );
    ckVectors->SetToolTip( _("If checked, vector fields are displayed.") );
    bSizerMain->Add( ckVectors, 0, wxALL|wxEXPAND, 5 );

    //add validator
    ckVectors->SetValidator(mafGUIValidator(this, ID_ACTIVATE_VECTORS,       
        ckVectors, &m_ActivateVectors));

    if (nVectors > 1)      
    {
      wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
      bSizer1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Vector Field:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

      m_ComboField_v = new wxComboBox( m_Gui, ID_VECTORFIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      m_ComboField_v->SetToolTip( _("Selects the vector field to be visualized.") );
      PopulateCombo(m_ComboField_v, true);  //at least one vector is available
      bSizer1->Add( m_ComboField_v, 1, wxALL, 1 );
      bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );

      //add validator
      m_ComboField_v->SetValidator(mafGUIValidator(this, ID_VECTORFIELD, m_ComboField_v, &m_VectorFieldIndex));
    }
    
    wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
    bSizer2->Add( new wxStaticText( m_Gui, wxID_ANY, _("Map by:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_ComboColorBy_v = new wxComboBox( m_Gui, ID_VECTOR_COLOR_MAPPING_MODE,wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    m_ComboColorBy_v->Append( _("Magnitude") );
    m_ComboColorBy_v->Append( _("X component") );
    m_ComboColorBy_v->Append( _("Y component") );
    m_ComboColorBy_v->Append( _("Z component") );
    m_ComboColorBy_v->SetToolTip(_("Specifies how the specified vector field should be mapped on to the surface.") );
    bSizer2->Add( m_ComboColorBy_v, 1, wxALL, 0 );
    bSizerMain->Add( bSizer2, 0, wxEXPAND, 5 );

    //add validator
    m_ComboColorBy_v->SetValidator(mafGUIValidator(this, ID_VECTOR_COLOR_MAPPING_MODE, m_ComboColorBy_v, &m_VectorColorMappingMode));
  }
  if (nVectors>0 || nScalars>0)
  {
    wxBoxSizer* bSizer3 = new wxBoxSizer( wxHORIZONTAL );    
    bSizer3->Add( new wxStaticText( m_Gui, wxID_ANY, _("LUT:")), 0, wxALL, 5 );
    mafGUILutSwatch* luts2 = new mafGUILutSwatch(m_Gui, ID_COLOR_MAPPING_LUT, wxDefaultPosition, wxSize(140, 18), wxTAB_TRAVERSAL | wxSIMPLE_BORDER );
    luts2->SetLut(m_ColorMappingLUT);
    luts2->SetEditable(true);
    luts2->SetListener(this);
    bSizer3->Add(luts2, 0, 0, 5 );
    bSizerMain->Add( bSizer3, 1, wxEXPAND, 5 );
  }

  wxCheckBox* chckShowSFLegend1 = new wxCheckBox( m_Gui, ID_SHOW_COLOR_MAPPING, _("Show Color Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
  chckShowSFLegend1->SetToolTip( _("If checked, mapping is displayed as a colored bar in the main view.") );
  bSizerMain->Add( chckShowSFLegend1, 0, wxALL|wxEXPAND, 5 );

  //add validator
  chckShowSFLegend1->SetValidator(mafGUIValidator(this, ID_SHOW_COLOR_MAPPING, chckShowSFLegend1, &m_ShowMapping));

  wxCheckBox* ckShowSurface = new wxCheckBox( m_Gui, ID_SHOW_SURFACE,_("Show Surface"), wxDefaultPosition, wxDefaultSize, 0 );
  ckShowSurface->SetToolTip( _("If checked, surface is displayed in the view") );
  bSizerMain->Add( ckShowSurface, 0, wxALL|wxEXPAND, 5 );

  //add validator
  ckShowSurface->SetValidator(mafGUIValidator(this, ID_SHOW_SURFACE, ckShowSurface, &m_ShowSurface));

  wxCheckBox* ckShowGlyphs = new wxCheckBox( m_Gui, ID_SHOW_GLYPHS,_("Show Glyphs"), wxDefaultPosition, wxDefaultSize, 0 );
  ckShowGlyphs->SetToolTip( _("If checked, glyphs are displayed in the view") );
  bSizerMain->Add( ckShowGlyphs, 0, wxALL|wxEXPAND, 5 );

  //add validator
  ckShowGlyphs->SetValidator(mafGUIValidator(this, ID_SHOW_GLYPHS, ckShowGlyphs, &m_ShowGlyphs));

  bSizerMain->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scale glyphs: "), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL, 5 );
  wxBoxSizer* bSizer4 = new wxBoxSizer( wxHORIZONTAL );
  wxTextCtrl *scale_text = new wxTextCtrl(m_Gui, wxID_ANY, "", wxDefaultPosition, wxSize(30,18), wxALL);
  mafGUIFloatSlider* slider = new mafGUIFloatSlider(m_Gui,ID_SCALESLIDER,0,1,10);
  slider->SetValidator(mafGUIValidator(this, ID_SCALESLIDER, slider, &m_ScalingValue, scale_text));
  slider->SetToolTip(_("Scale glyphs.") );
  slider->SetNumberOfSteps(10);
  bSizer4->Add(scale_text, 0, wxALL, 5);
  bSizer4->Add(slider, 1, wxALL, 0);
  bSizerMain->Add( bSizer4, 0, wxEXPAND, 0);
  
  m_Gui->Add(bSizerMain);

  m_Gui->Update();

  return m_Gui;
}


//----------------------------------------------------------------------------
void medPipeVectorFieldMapWithArrows::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{			
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
      switch(e->GetId())
      {
      case ID_ACTIVATE_SCALARS:
        {
          m_ActivateVectors = 0;

          m_ComboField_s->Enable(true);
          m_ComboField_v->Enable(false);
          m_ComboColorBy_s->Enable(true);
          m_ComboColorBy_v->Enable(false);
          
          m_Gui->Update();
          UpdateVTKPipe(); 
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
          return;
        }
      break;
      case ID_ACTIVATE_VECTORS:
        {
          m_ActivateScalars = 0;

          m_ComboField_s->Enable(false);
          m_ComboField_v->Enable(true);
          m_ComboColorBy_s->Enable(false);
          m_ComboColorBy_v->Enable(true);

          m_Gui->Update();
          UpdateVTKPipe(); 
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
          return;
        }
      break;
      default:
        {
          UpdateVTKPipe(); 
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
          return;
        }
      }
    }     
  }

  //forward it to our listener to deal with it
  mafEventMacro(*maf_event);
}

//------------------------------------------------------------------------
//Constructs VTK pipeline.
void medPipeVectorFieldMapWithArrows::CreateVTKPipe()
//------------------------------------------------------------------------
{ 
  // ----------------------
  // PHASE 1: BUILD SURFACE  
  // ----------------------

  // Build LUT
  m_ColorMappingLUT = vtkLookupTable::New();
  lutPreset(12, m_ColorMappingLUT); //initialize LUT to SAR (it has index 12)  
  m_ColorMappingLUT->Build(); 

  vtkGeometryFilter* filter = vtkGeometryFilter::New();
  filter->SetInput(m_Vme->GetOutput()->GetVTKData());

  m_SurfaceMapper = vtkPolyDataMapper::New();
  m_SurfaceMapper->SetInput(filter->GetOutput());
  m_SurfaceMapper->ImmediateModeRenderingOn();
  m_SurfaceMapper->SetScalarModeToUsePointFieldData();
  m_SurfaceMapper->SetColorModeToMapScalars();
  m_SurfaceMapper->SetLookupTable(m_ColorMappingLUT);

  m_SurfaceActor = vtkActor::New();
  m_SurfaceActor->SetMapper(m_SurfaceMapper);
  m_SurfaceActor->SetPickable(0);   //make it faster

  // Build scalar field map
  m_MappingActor = vtkScalarBarActor::New();
  m_MappingActor->SetLookupTable(m_SurfaceMapper->GetLookupTable());
  ((vtkActor2D*)m_MappingActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  ((vtkActor2D*)m_MappingActor)->GetPositionCoordinate()->SetValue(0.1,0.01);
  m_MappingActor->SetOrientationToHorizontal();
  m_MappingActor->SetWidth(0.8);
  m_MappingActor->SetHeight(0.12);  
  m_MappingActor->SetLabelFormat("%6.3g");
  m_MappingActor->SetPickable(0);   //make it faster

  // Add to assembly
  m_RenFront->AddActor2D(m_MappingActor);
  m_AssemblyFront->AddPart(m_SurfaceActor); 

  // ---------------------
  // PHASE 2: BUILD GLYPHS
  // ---------------------

  int nVectors = GetNumberOfVectors();

  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  vtkPointData* pd = ds->GetPointData();
  if (pd == NULL)
    return;

  vtkDataArray* da_normals = pd->GetNormals();
  if (!da_normals)
  {
    vtkPolyDataNormals* f_normals;
    vtkNEW(f_normals);
    f_normals->SetInput(filter->GetOutput());
    f_normals->ComputeCellNormalsOff();
    f_normals->ComputePointNormalsOn();
    f_normals->Update();
    da_normals= f_normals->GetOutput()->GetPointData()->GetNormals();
    da_normals->SetName("Normals");
    pd->AddArray(da_normals);
    vtkDEL(f_normals);
  }
  
  int nOfComponents = -1;
  vtkDataArray* da = pd->GetArray(m_VectorFieldIndex);
  nOfComponents = da->GetNumberOfComponents();
  if (nOfComponents!=1 && nOfComponents!=3)
    return;

  float distance = 0;
  for (int i=0;i<ds->GetNumberOfPoints()-2;i++)
  {
    double p1[3];
    double p2[3];
    ds->GetPoint(i,p1);
    ds->GetPoint(i+1,p2);
    double d = sqrt(pow(p1[0]-p2[0],2)+pow(p1[1]-p2[1],2)+pow(p1[2]-p2[2],2));
    distance += d;
  }
  distance = distance/(ds->GetNumberOfPoints()-1);
  float max_norm = da->GetMaxNorm();
  float scale_factor = distance/max_norm;
  
  vtkArrowSource *arrow = vtkArrowSource::New();
  arrow->SetTipResolution(6);
  arrow->SetTipRadius(.1);
  arrow->SetTipLength(.35);
  arrow->SetShaftResolution(6);
  arrow->SetShaftRadius(.03);

  m_Glyph = vtkGlyph3D::New();
  m_Glyph->SetInput(m_Vme->GetOutput()->GetVTKData());        
  m_Glyph->SetSource(arrow->GetOutput());

  m_Glyph->SetScaleFactor(scale_factor);
  m_Glyph->SetRange(m_SurfaceMapper->GetLookupTable()->GetRange());

  m_Glyph->SelectInputNormals(da_normals->GetName());
  m_Glyph->SelectInputVectors(GetVectorFieldName(m_VectorFieldIndex));
  m_Glyph->SelectInputScalars(GetScalarFieldName(m_ScalarFieldIndex));

  m_Glyph->SetVectorModeToUseVector();
  m_Glyph->SetScaleModeToScaleByVector();
  m_Glyph->SetColorModeToColorByVector();
  m_Glyph->Update();

  m_GlyphMapper = vtkPolyDataMapper::New();
  m_GlyphMapper->SetInput(m_Glyph->GetOutput());
  m_GlyphMapper->ImmediateModeRenderingOn();
  m_GlyphMapper->SetScalarRange(m_SurfaceMapper->GetLookupTable()->GetRange());
  m_GlyphMapper->SetLookupTable(m_ColorMappingLUT);

  m_GlyphActor = vtkActor::New();
  m_GlyphActor->SetMapper(m_GlyphMapper);
  m_GlyphActor->SetPickable(0);   //make it faster

  m_AssemblyFront->AddPart(m_GlyphActor); 
  
  arrow->Delete(); 
  filter->Delete();
}

//------------------------------------------------------------------------
//Updates VTK pipeline (setting radius, etc.). 
void medPipeVectorFieldMapWithArrows::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  vtkPointData* pd = ds->GetPointData();
  if (pd == NULL)
    return;
  
  const char* field_name;
  vtkDataArray* da;
  double sr[2];

  // -----------------------
  // PHASE 1: UPDATE SURFACE
  // -----------------------
  
  if (m_ActivateVectors)
  {
    field_name = GetVectorFieldName(m_VectorFieldIndex);
    m_SurfaceMapper->SelectColorArray(field_name);
    da = pd->GetVectors(field_name);
    da->GetRange(sr, m_VectorColorMappingMode - CMM_X);
    if (m_VectorColorMappingMode == CMM_MAGNITUDE)    
    {
      // Magnitude
      m_ColorMappingLUT->SetVectorModeToMagnitude();    
    }
    else
    {
      // X, Y or Z component
      m_ColorMappingLUT->SetVectorModeToComponent();
      m_ColorMappingLUT->SetVectorComponent(m_VectorColorMappingMode - CMM_X);    
    }
  }
  else if (m_ActivateScalars)
  {
    field_name = GetScalarFieldName(m_ScalarFieldIndex);
    m_SurfaceMapper->SelectColorArray(field_name);
    da = pd->GetScalars(field_name);
    da->GetRange(sr);
  }
  else
  {
    return;
  }   
   
  m_ColorMappingLUT->SetTableRange(sr);
  m_SurfaceMapper->SetScalarRange(sr);
  m_SurfaceMapper->Update();  
  
  m_SurfaceActor->SetVisibility(m_ShowSurface);
  m_MappingActor->SetVisibility(m_ShowMapping); 

  // ----------------------
  // PHASE 2: UPDATE GLYPHS
  // ----------------------

  int nOfComponents = -1;
  nOfComponents = da->GetNumberOfComponents();
  if (nOfComponents!=1 && nOfComponents!=3)
    return;
 
  float distance = 0;
  for (int i=0;i<ds->GetNumberOfPoints()-2;i++)
  {
    double p1[3];
    double p2[3];
    ds->GetPoint(i,p1);
    ds->GetPoint(i+1,p2);
    double d = sqrt(pow(p1[0]-p2[0],2)+pow(p1[1]-p2[1],2)+pow(p1[2]-p2[2],2));
    distance += d;
  }
  distance = distance/(ds->GetNumberOfPoints()-1);
  double* range = m_SurfaceMapper->GetLookupTable()->GetRange();
  float max_norm = 0;
  if (nOfComponents==1)
  {
    max_norm = range[1];
  }
  else
  {
    max_norm = da->GetMaxNorm();
  }
  float scale_factor = distance/max_norm;

  m_Glyph->SetScaleFactor(m_ScalingValue*scale_factor);
  m_Glyph->SetRange(range);
    
  if (nOfComponents==3)
  {
    m_Glyph->SelectInputVectors(GetVectorFieldName(m_VectorFieldIndex));
    m_Glyph->SetVectorModeToUseVector();
    m_Glyph->SetScaleModeToScaleByVector();
    m_Glyph->SetColorModeToColorByVector();
  }
  else
  {
    m_Glyph->SelectInputScalars(GetScalarFieldName(m_ScalarFieldIndex));
    m_Glyph->SetVectorModeToUseNormal();
    m_Glyph->SetScaleModeToScaleByScalar();
    m_Glyph->SetColorModeToColorByScalar();
  }

  m_GlyphMapper->SetScalarRange(m_SurfaceMapper->GetLookupTable()->GetRange());
  m_GlyphMapper->SetLookupTable(m_ColorMappingLUT);

  m_GlyphActor->SetVisibility(m_ShowGlyphs);

  m_Glyph->Update();
}