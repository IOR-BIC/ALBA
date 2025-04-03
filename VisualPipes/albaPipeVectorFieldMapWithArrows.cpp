/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorFieldMapWithArrows
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaPipeVectorFieldMapWithArrows.h"

/* ALBA */
#include "albaSceneNode.h"
#include "albaPipeFactory.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaGUILutSwatch.h"
#include "albaGUILutPreset.h"
#include "albaGUIFloatSlider.h"
#include "albaVME.h"

/* VTK */
#include "vtkArrowSource.h"
#include "vtkConeSource.h"
#include "vtkLineSource.h"
#include "vtkGlyph3D.h"

#include "vtkALBAAssembly.h"
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

#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"
#include <vtkAssignAttribute.h>

#include "wx/busyinfo.h"
#include <float.h>
#include "vtkALBASmartPointer.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVectorFieldMapWithArrows);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeVectorFieldMapWithArrows::albaPipeVectorFieldMapWithArrows() : albaPipeVectorField()
//----------------------------------------------------------------------------
{     
  m_ColorMappingLUT = NULL;  
  m_ShowMapping = 1;  
  m_ShowSurface = 1;
  m_ShowGlyphs = 1;
  m_ScalingValue = 1;
  m_EnableMap = 1;

  m_GlyphRes = 6;
  m_GlyphLength = 0.175;
  m_GlyphRadius = 0.05;
    
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

  m_Arrow = NULL;
  m_Line = NULL;

  m_GlyphType = GLYPH_ARROWS;

  m_ComboField_s = NULL;
  m_ComboField_v = NULL;
  m_ComboColorBy_s = NULL;
  m_ComboColorBy_v = NULL;
  m_EdRadius = NULL;
  m_EdRes = NULL;
  m_EdLength = NULL;

  m_DataType = POINT_DATA;

  Superclass::SetCreateVTKPipeAlways(true);
}

//----------------------------------------------------------------------------
albaPipeVectorFieldMapWithArrows::~albaPipeVectorFieldMapWithArrows()
//----------------------------------------------------------------------------
{ 
  m_AssemblyFront->RemovePart(m_GlyphActor); 
  m_AssemblyFront->RemovePart(m_SurfaceActor);
  m_RenFront->RemoveActor2D(m_MappingActor);  
  
  vtkDEL(m_MappingActor);

  vtkDEL(m_Arrow);
  vtkDEL(m_Line);
  
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
  cppDEL(m_EdRadius);
  cppDEL(m_EdRes);
  cppDEL(m_EdLength);
}
//----------------------------------------------------------------------------
albaGUI *albaPipeVectorFieldMapWithArrows::CreateGui()
//----------------------------------------------------------------------------
{ 
  // Get number of vectors and scalars
  int nVectors = GetNumberOfVectors();
  int nScalars = GetNumberOfScalars();
  
  m_Gui = new albaGUI(this);

  // If no fields found don't build widgets
  if (nVectors==0 && nScalars==0)
  {
    m_Gui->Label("No vector or scalar fields to visualize.", false);
    return m_Gui;
  }

  if (nVectors==0 && nScalars>0)
  {
    m_ActivateVectors = 0;
    m_ActivateScalars = 1;
  }

  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );
     
  m_Gui->Divider();
  
  // -------------
  // SCALAR FIELDS
  // -------------
  if (nScalars>0)
  {
    wxStaticBoxSizer* bSizerScalar = new wxStaticBoxSizer(new wxStaticBox( m_Gui, wxID_ANY, wxT("Scalar fields") ), wxVERTICAL );

    wxCheckBox* ckScalars = new wxCheckBox( m_Gui, ID_ACTIVATE_SCALARS, _("Activate scalar fields"), wxDefaultPosition, wxDefaultSize, 0 );
    ckScalars->SetToolTip( _("If checked, scalar fields are displayed.") );
    ckScalars->SetValidator(albaGUIValidator(this, ID_ACTIVATE_SCALARS, ckScalars, &m_ActivateScalars));

    bSizerScalar->Add( ckScalars, 0, wxALL|wxEXPAND, 1 );

    wxBoxSizer* bSizer_s1 = new wxBoxSizer( wxHORIZONTAL );
    bSizer_s1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scalar Field:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_ComboField_s = new wxComboBox( m_Gui, ID_SCALARFIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
    m_ComboField_s->SetToolTip( _("Selects the scalar field to be visualized.") );
    PopulateCombo(m_ComboField_s, false);  //at least one scalar is available
    m_ComboField_s->SetValidator(albaGUIValidator(this, ID_SCALARFIELD, m_ComboField_s, &m_ScalarFieldIndex));
    m_ComboField_s->Enable(false);

    bSizer_s1->Add( m_ComboField_s, 1, wxALL, 1 );
    bSizerScalar->Add( bSizer_s1, 0, wxEXPAND, 1 );
    
    wxBoxSizer* bSizer_s2 = new wxBoxSizer( wxHORIZONTAL );
    bSizer_s2->Add( new wxStaticText( m_Gui, wxID_ANY, _("Color by:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_ComboColorBy_s = new wxComboBox( m_Gui, ID_SCALAR_COLOR_MAPPING_MODE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    m_ComboColorBy_s->Append( _("Scalar value") );
    m_ComboColorBy_s->SetToolTip(_("Specifies how the specified scalar field should be mapped on to the surface.") );
    m_ComboColorBy_s->SetValidator(albaGUIValidator(this, ID_SCALAR_COLOR_MAPPING_MODE, m_ComboColorBy_s, &m_ScalarColorMappingMode));
    m_ComboColorBy_s->Enable(false);

    bSizer_s2->Add( m_ComboColorBy_s, 1, wxALL, 0 );

    bSizerScalar->Add(bSizer_s2, 0, wxEXPAND, 1);
    bSizerMain->Add( bSizerScalar, 0, wxEXPAND, 5 );
  }
  // -------------
  
  // -------------
  // VECTOR FIELDS
  // -------------
  if (nVectors>0)
  {
    wxStaticBoxSizer* bSizerVector = new wxStaticBoxSizer( new wxStaticBox( m_Gui, wxID_ANY, wxT("Vector fields") ), wxVERTICAL );
    
    wxCheckBox* ckVectors = new wxCheckBox( m_Gui, ID_ACTIVATE_VECTORS, _("Activate vector fields"), wxDefaultPosition, wxDefaultSize, 0 );
    ckVectors->SetToolTip( _("If checked, vector fields are displayed.") );
    ckVectors->SetValidator(albaGUIValidator(this, ID_ACTIVATE_VECTORS, ckVectors, &m_ActivateVectors));

    bSizerVector->Add( ckVectors, 0, wxALL|wxEXPAND, 1 );

    wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
    bSizer1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Vector Field:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_ComboField_v = new wxComboBox( m_Gui, ID_VECTORFIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
    m_ComboField_v->SetToolTip( _("Selects the vector field to be visualized.") );
    PopulateCombo(m_ComboField_v, true);  //at least one vector is available
    m_ComboField_v->SetValidator(albaGUIValidator(this, ID_VECTORFIELD, m_ComboField_v, &m_VectorFieldIndex));

    bSizer1->Add( m_ComboField_v, 1, wxALL, 1 );
    bSizerVector->Add( bSizer1, 0, wxEXPAND, 1 );
    
    wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
    bSizer2->Add( new wxStaticText( m_Gui, wxID_ANY, _("Color by:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_ComboColorBy_v = new wxComboBox( m_Gui, ID_VECTOR_COLOR_MAPPING_MODE,wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    m_ComboColorBy_v->Append( _("Magnitude") );
    m_ComboColorBy_v->Append( _("X component") );
    m_ComboColorBy_v->Append( _("Y component") );
    m_ComboColorBy_v->Append( _("Z component") );
    m_ComboColorBy_v->SetToolTip(_("Specifies how the specified vector field should be mapped on to the surface.") );
    m_ComboColorBy_v->SetValidator(albaGUIValidator(this, ID_VECTOR_COLOR_MAPPING_MODE, m_ComboColorBy_v, &m_VectorColorMappingMode));

    bSizer2->Add( m_ComboColorBy_v, 1, wxALL, 0 );
    bSizerVector->Add( bSizer2, 1, wxEXPAND, 0);
    bSizerMain->Add( bSizerVector, 0, wxEXPAND, 5 );
  }
  // ---------------

  // ---------------
  // VIEW PROPERTIES
  // ---------------
  wxStaticBoxSizer* bSizerProperties = new wxStaticBoxSizer( new wxStaticBox( m_Gui, wxID_ANY, wxT("View properties") ), wxVERTICAL );

  wxCheckBox* ckEnableMap = new wxCheckBox( m_Gui, ID_ENABLE_MAP,_("Enable Color Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
  ckEnableMap->SetToolTip( _("If checked, glyphs and surface are colored using a mapping of the specified LUT") );
  ckEnableMap->SetValidator(albaGUIValidator(this, ID_ENABLE_MAP, ckEnableMap, &m_EnableMap));

  bSizerProperties->Add( ckEnableMap, 0, wxALL|wxEXPAND, 3 );

  wxBoxSizer* bSizer3 = new wxBoxSizer( wxHORIZONTAL );    
  bSizer3->Add( new wxStaticText( m_Gui, wxID_ANY, _("LUT:")), 0, wxALL, 5 );
  albaGUILutSwatch* luts2 = new albaGUILutSwatch(m_Gui, ID_COLOR_MAPPING_LUT, wxDefaultPosition, wxSize(140, 18), wxTAB_TRAVERSAL | wxSIMPLE_BORDER );
  luts2->SetLut(m_ColorMappingLUT);
  luts2->SetEditable(true);
  luts2->SetListener(this);
  bSizer3->Add(luts2, 0, 0, 5 );
  bSizerProperties->Add( bSizer3, 0, wxEXPAND, 1 );

  wxCheckBox* chckShowSFLegend1 = new wxCheckBox( m_Gui, ID_SHOW_COLOR_MAPPING, _("Show Color Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
  chckShowSFLegend1->SetToolTip( _("If checked, mapping is displayed as a colored bar in the main view.") );
  chckShowSFLegend1->SetValidator(albaGUIValidator(this, ID_SHOW_COLOR_MAPPING, chckShowSFLegend1, &m_ShowMapping));

  bSizerProperties->Add( chckShowSFLegend1, 0, wxALL|wxEXPAND, 3 );

  wxCheckBox* ckShowSurface = new wxCheckBox( m_Gui, ID_SHOW_SURFACE,_("Show Surface"), wxDefaultPosition, wxDefaultSize, 0 );
  ckShowSurface->SetToolTip( _("If checked, surface is displayed in the view") );
  ckShowSurface->SetValidator(albaGUIValidator(this, ID_SHOW_SURFACE, ckShowSurface, &m_ShowSurface));

  bSizerProperties->Add( ckShowSurface, 0, wxALL|wxEXPAND, 3 );

  wxCheckBox* ckShowGlyphs = new wxCheckBox( m_Gui, ID_SHOW_GLYPHS,_("Show Glyphs"), wxDefaultPosition, wxDefaultSize, 0 );
  ckShowGlyphs->SetToolTip( _("If checked, glyphs are displayed in the view") );
  ckShowGlyphs->SetValidator(albaGUIValidator(this, ID_SHOW_GLYPHS, ckShowGlyphs, &m_ShowGlyphs));

  bSizerProperties->Add( ckShowGlyphs, 0, wxALL|wxEXPAND, 3 );

  // -----------
  // GLYPH SHAPE
  // -----------
  wxStaticBoxSizer* sbSizer = new wxStaticBoxSizer( new wxStaticBox( m_Gui, wxID_ANY, _("Glyph shape") ), wxVERTICAL );

  sbSizer->Add( new wxStaticText( m_Gui, wxID_ANY, _("Dimension"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL, 5 );
  wxBoxSizer* bSizer4 = new wxBoxSizer( wxHORIZONTAL );
  wxTextCtrl *scale_text = new wxTextCtrl(m_Gui, wxID_ANY, "", wxDefaultPosition, wxSize(30,18), wxALL);
  albaGUIFloatSlider* slider = new albaGUIFloatSlider(m_Gui,ID_SCALESLIDER,0,1,10);
  slider->SetValidator(albaGUIValidator(this, ID_SCALESLIDER, slider, &m_ScalingValue, scale_text));
  slider->SetToolTip(_("Change the glyphs dimensions, also scaled by the selected scaling component.") );
  slider->SetNumberOfSteps(10);
  bSizer4->Add(scale_text, 0, wxALL, 5);
  bSizer4->Add(slider, 1, wxALL, 0);
  sbSizer->Add( bSizer4, 0, wxEXPAND, 3);

  wxBoxSizer* bSizer5 = new wxBoxSizer( wxHORIZONTAL );
  
  bSizer5->Add( new wxStaticText( m_Gui, wxID_ANY, _("Radius:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );
  m_EdRadius = new wxTextCtrl( m_Gui, ID_GLYPH_RADIUS);
  m_EdRadius->SetToolTip( _("Specifies the radius of glyph, i.e., the radius of arrows. ") );
  bSizer5->Add( m_EdRadius, 1, wxALL, 1 );
  m_EdRadius->SetValidator(albaGUIValidator(this, ID_GLYPH_RADIUS, m_EdRadius, &m_GlyphRadius, 1e-8, 1000));

  wxBoxSizer* bSizer51 = new wxBoxSizer( wxHORIZONTAL );
  bSizer51->Add( new wxStaticText( m_Gui, wxID_ANY, _("Resolution:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );
  m_EdRes = new wxTextCtrl( m_Gui, ID_GLYPH_RESOLUTION);
  m_EdRes->SetToolTip( _("Specifies the resolution of glyphs, i.e., number of sides of cones, etc.") );
  bSizer51->Add( m_EdRes, 1, wxALL, 1 );
  m_EdRes->SetValidator(albaGUIValidator(this, ID_GLYPH_RESOLUTION, m_EdRes, &m_GlyphRes, 3, 100));

  wxBoxSizer* bSizer52 = new wxBoxSizer( wxHORIZONTAL );
  bSizer52->Add( new wxStaticText( m_Gui, wxID_ANY, _("Length:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );
  m_EdLength = new wxTextCtrl( m_Gui, ID_GLYPH_LENGTH);
  m_EdLength->SetToolTip( _("Specifies the length of glyph, which will be scaled by the selected scaling component.") );
  bSizer52->Add( m_EdLength, 1, wxALL, 1 );
  m_EdLength->SetValidator(albaGUIValidator(this, ID_GLYPH_LENGTH, m_EdLength, &m_GlyphLength, 1e-8, 1000));  

  wxBoxSizer* bSizer53 = new wxBoxSizer( wxHORIZONTAL );
  bSizer53->Add( new wxStaticText( m_Gui, wxID_ANY, _("Glyph Type:"), wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );
  wxComboBox* comboGlyphs = new wxComboBox( m_Gui, ID_GLYPH_TYPE, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0, NULL, wxCB_READONLY );
  comboGlyphs->Append( _("Lines") );
  comboGlyphs->Append( _("Arrows") );
  comboGlyphs->SetToolTip( _("Specifies glyph type which should be used for the visualization of the selected vector field.") );
  comboGlyphs->SetValidator(albaGUIValidator(this, ID_GLYPH_TYPE, comboGlyphs, &m_GlyphType));
  bSizer53->Add( comboGlyphs, 1, wxALL, 1 );
    
  sbSizer->Add( bSizer5, 0, wxALL, 1 );
  sbSizer->Add( bSizer52, 0, wxALL, 1);
  sbSizer->Add( bSizer51, 0, wxALL, 1);
  sbSizer->Add( bSizer53, 0, wxALL, 1 );
 
  bSizerProperties->Add(sbSizer, 0, wxEXPAND, 5);
  // ---------
  // ---------
  
  bSizerMain->Add(bSizerProperties, 0, wxALL|wxEXPAND, 1);
  
  m_Gui->Add(bSizerMain);
  m_Gui->Update();

  return m_Gui;
}


//----------------------------------------------------------------------------
void albaPipeVectorFieldMapWithArrows::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{			
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
      switch(e->GetId())
      {
      case ID_ACTIVATE_SCALARS:
        {
          // activate scalars and update gui
          if (m_ActivateVectors==0 && m_ActivateScalars==0)
          {
            m_ActivateScalars = !m_ActivateScalars;
            m_Gui->Update();
            return;
          }

          // deactivate vectors
          m_ActivateVectors = 0;

          if (m_ComboField_s!=NULL) {
            m_ComboField_s->Enable(true);
          }
          if (m_ComboField_v!=NULL) {
            m_ComboField_v->Enable(false);
          }
          if (m_ComboColorBy_s!=NULL) {
            m_ComboColorBy_s->Enable(true);
          }
          if (m_ComboColorBy_v!=NULL) {
            m_ComboColorBy_v->Enable(false);
          }
          
          m_Gui->Update();
          UpdateVTKPipe(); 
					GetLogicManager()->CameraUpdate();
          return;
        }
      break;
      case ID_ACTIVATE_VECTORS:
        {
          // activate vectors and update gui
          if (m_ActivateVectors==0 && m_ActivateScalars==0)
          {
            m_ActivateVectors = !m_ActivateVectors;
            m_Gui->Update();
            return;
          }

          // deactivate scalars
          m_ActivateScalars = 0;

          if (m_ComboField_s!=NULL) {
            m_ComboField_s->Enable(false);
          }
          if (m_ComboField_v!=NULL) {
            m_ComboField_v->Enable(true);
          }
          if (m_ComboColorBy_s!=NULL) {
            m_ComboColorBy_s->Enable(false);
          }
          if (m_ComboColorBy_v!=NULL) {
            m_ComboColorBy_v->Enable(true);
          }

          m_Gui->Update();
          UpdateVTKPipe(); 
					GetLogicManager()->CameraUpdate();
          return;
        }
      break;
      case ID_GLYPH_TYPE:
        {
          // change glyph type
          if (m_GlyphType==GLYPH_LINES)
          {
            m_EdRadius->Enable(false);
            m_EdRes->Enable(false);
            m_EdLength->Enable(false);
          }
          else
          {
            m_EdRadius->Enable(true);
            m_EdRes->Enable(true);
            m_EdLength->Enable(true);
          }
          UpdateVTKPipe(); 
					GetLogicManager()->CameraUpdate();
          return;
        }
      break;
      default:
        {
          UpdateVTKPipe(); 
					GetLogicManager()->CameraUpdate();
          return;
        }
      }
    }     
  }

  //forward it to our listener to deal with it
  albaEventMacro(*alba_event);
}

//------------------------------------------------------------------------
void albaPipeVectorFieldMapWithArrows::CreateVTKPipe()
//------------------------------------------------------------------------
{ 
  if (!m_Vme->GetOutput()->GetVTKData()) {
    return;
  }

  m_Vme->Update();
  m_Vme->GetOutput()->Update();

  int nScalars = GetNumberOfScalars();
  int nVectors = GetNumberOfVectors();

  if (nScalars==0 && nVectors==0) {
    return;
  }

  // ----------------------
  // PHASE 1: BUILD SURFACE  
  // ----------------------

  // build LUT
  m_ColorMappingLUT = vtkLookupTable::New();
  lutPreset(12, m_ColorMappingLUT); 
  m_ColorMappingLUT->Build(); 

  // transform dataset to polydata
  vtkGeometryFilter* filter = vtkGeometryFilter::New();
  filter->SetInputData(m_Vme->GetOutput()->GetVTKData());

  // build surface mapper
  m_SurfaceMapper = vtkPolyDataMapper::New();
  m_SurfaceMapper->SetInputConnection(filter->GetOutputPort());

  // assign right mapping mode
  if (m_DataType==POINT_DATA) {
    m_SurfaceMapper->SetScalarModeToUsePointFieldData();
  }
  else if (m_DataType==CELL_DATA)  {
    m_SurfaceMapper->SetScalarModeToUseCellFieldData();
  }

  // initializa mapper
  m_SurfaceMapper->SetColorModeToMapScalars();
  m_SurfaceMapper->SetLookupTable(m_ColorMappingLUT);
  m_SurfaceActor = vtkActor::New();
  m_SurfaceActor->SetMapper(m_SurfaceMapper);
  m_SurfaceActor->SetPickable(0);   //make it faster

  // build scalar field map
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

  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  vtkPointData* pd = ds->GetPointData();
  vtkCellData* cd = ds->GetCellData();
  if ( pd == NULL || cd==NULL )
    return;

  // get normals from points or cells
  vtkDataArray* da_normals;
  if (m_DataType==POINT_DATA) {
    da_normals = pd->GetNormals();
  }
  else if (m_DataType==CELL_DATA)
  {
    da_normals = cd->GetNormals();
  }

  // build normals if they do not exist
  if (!da_normals)
  {
    vtkPolyDataNormals* f_normals;
    vtkNEW(f_normals);
    f_normals->SetInputConnection(filter->GetOutputPort());

    // set normals to points or cells
    if (m_DataType==POINT_DATA)
    {
      f_normals->ComputeCellNormalsOff();
      f_normals->ComputePointNormalsOn();
      f_normals->Update();
      da_normals= f_normals->GetOutput()->GetPointData()->GetNormals();
      da_normals->SetName("Normals");
      pd->AddArray(da_normals);
      pd->Update();
    }
    else if (m_DataType==CELL_DATA)
    {
      f_normals->ComputeCellNormalsOn();
      f_normals->ComputePointNormalsOff();
      f_normals->Update();
      da_normals= f_normals->GetOutput()->GetCellData()->GetNormals();
      da_normals->SetName("Normals");
      cd->AddArray(da_normals);
      cd->Update();
    }
    
    vtkDEL(f_normals);
  }
  
  // get data array from points or cells
  int nOfComponents = -1;
  vtkDataArray* da;
  if (m_DataType==POINT_DATA)
  {
    da = pd->GetArray(m_VectorFieldIndex);
  }
  else if (m_DataType==CELL_DATA)
  {
    da = cd->GetArray(m_VectorFieldIndex);
  }
  nOfComponents = da->GetNumberOfComponents();
  if (nOfComponents!=1 && nOfComponents!=3)
    return;

  // scale absolut value of scalar components to view dimensions
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
  
  // arrow glyph
  m_Arrow = vtkArrowSource::New();
  m_Arrow->SetTipResolution(m_GlyphRes);
  m_Arrow->SetTipRadius(m_GlyphRadius);
  m_Arrow->SetTipLength(m_GlyphLength);
  m_Arrow->SetShaftResolution(m_GlyphRes);
  m_Arrow->SetShaftRadius(m_GlyphRadius*0.3);

  // line glyph
  m_Line = vtkLineSource::New(); 
  m_Line->SetPoint1(0.0, 0.0, 0.0);
  m_Line->SetPoint2(1.0, 0.0, 0.0);
  m_Line->SetResolution(m_GlyphRes);

  // build glyph
	m_AttributeAssigner = vtkAssignAttribute::New();
	m_AttributeAssigner->SetInputData(m_Vme->GetOutput()->GetVTKData());
	m_AttributeAssigner->Assign(da_normals->GetName(), vtkDataSetAttributes::NORMALS, m_DataType);
	m_AttributeAssigner->Assign(GetVectorFieldName(m_VectorFieldIndex), vtkDataSetAttributes::VECTORS, m_DataType);
	m_AttributeAssigner->Assign(GetScalarFieldName(m_ScalarFieldIndex), vtkDataSetAttributes::SCALARS, m_DataType);

  m_Glyph = vtkGlyph3D::New();
  m_Glyph->SetInputConnection(m_AttributeAssigner->GetOutputPort());        
  m_Glyph->SetSourceConnection(m_Arrow->GetOutputPort());

  m_Glyph->SetScaleFactor(scale_factor);
  m_Glyph->SetRange(m_SurfaceMapper->GetLookupTable()->GetRange());


  m_Glyph->SetVectorModeToUseVector();
  m_Glyph->SetScaleModeToScaleByVector();
  m_Glyph->SetColorModeToColorByVector();
  m_Glyph->Update();

  // build mapper  
  m_GlyphMapper = vtkPolyDataMapper::New();
  m_GlyphMapper->SetInputConnection(m_Glyph->GetOutputPort());
  m_GlyphMapper->SetScalarRange(m_SurfaceMapper->GetLookupTable()->GetRange());
  m_GlyphMapper->SetLookupTable(m_ColorMappingLUT);

  // build actor
  m_GlyphActor = vtkActor::New();
  m_GlyphActor->SetMapper(m_GlyphMapper);
  m_GlyphActor->SetPickable(0);   //make it faster

  m_AssemblyFront->AddPart(m_GlyphActor); 
  
  filter->Delete();
}
//------------------------------------------------------------------------
//Updates VTK pipeline (setting radius, etc.). 
void albaPipeVectorFieldMapWithArrows::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  if (!m_Vme->GetOutput()->GetVTKData()) {
    return;
  }

  m_Vme->Update();
  m_Vme->GetOutput()->Update();

  int nScalars = GetNumberOfScalars();
  int nVectors = GetNumberOfVectors();

  if (nScalars==0 && nVectors==0) {
    return;
  }

  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  vtkPointData* pd = ds->GetPointData();
  vtkCellData* cd = ds->GetCellData();
  if (pd == NULL || cd == NULL)
    return;

  const char* field_name;
  vtkDataArray* da;
  double sr[2];

  // -----------------------
  // PHASE 1: UPDATE NORMALS
  // -----------------------

  // transform dataset to polydata
  vtkGeometryFilter* filter = vtkGeometryFilter::New();
  filter->SetInputData(ds);
  
  // get normals from cells or points
  vtkDataArray* da_normals;
  if (m_DataType==POINT_DATA) {
    da_normals = pd->GetNormals();
  }
  else if (m_DataType==CELL_DATA)
  {
    da_normals = cd->GetNormals();
  }

  // build them if they do not exist
  if (!da_normals)
  {
    vtkPolyDataNormals* f_normals;
    vtkNEW(f_normals);
    f_normals->SetInputConnection(filter->GetOutputPort());

    // set them to points or cells
    if (m_DataType==POINT_DATA)
    {
      f_normals->ComputeCellNormalsOff();
      f_normals->ComputePointNormalsOn();
      f_normals->Update();
      da_normals= f_normals->GetOutput()->GetPointData()->GetNormals();
      da_normals->SetName("Normals");
      pd->AddArray(da_normals);
      pd->Update();
    }
    else if (m_DataType==CELL_DATA)
    {
      f_normals->ComputeCellNormalsOn();
      f_normals->ComputePointNormalsOff();
      f_normals->Update();
      da_normals= f_normals->GetOutput()->GetCellData()->GetNormals();
      da_normals->SetName("Normals");
      cd->AddArray(da_normals);
      cd->Update();
    }

    vtkDEL(f_normals);
  }
  if (filter)
  {
    filter->Delete();
  }

  // -----------------------
  // PHASE 1: UPDATE SURFACE
  // -----------------------

  // case 1: vectors are activated
  if (m_ActivateVectors)
  {
    // get field from cells or points
    field_name = GetVectorFieldName(m_VectorFieldIndex);
    m_SurfaceMapper->SelectColorArray(field_name);
    if (m_DataType==POINT_DATA)
    {
      da = pd->GetVectors(field_name);
    }
    else if (m_DataType==CELL_DATA)
    {
      da = cd->GetVectors(field_name);
    }
    
    // get range
    da->GetRange(sr, m_VectorColorMappingMode - CMM_X);
    if (m_VectorColorMappingMode == CMM_MAGNITUDE)    
    {
      // set magnitude
      m_ColorMappingLUT->SetVectorModeToMagnitude();    
    }
    else
    {
      // set X, Y or Z component
      m_ColorMappingLUT->SetVectorModeToComponent();
      m_ColorMappingLUT->SetVectorComponent(m_VectorColorMappingMode - CMM_X);    
    }
  }
  // case 2: Scalars are activated
  else if (m_ActivateScalars)
  {
    // get field from points or cells
    field_name = GetScalarFieldName(m_ScalarFieldIndex);
    m_SurfaceMapper->SelectColorArray(field_name);
    if (m_DataType==POINT_DATA)
    {
      da = pd->GetScalars(field_name);
    }
    else if (m_DataType==CELL_DATA)
    {
      da = cd->GetScalars(field_name);
    }

    // get range
    da->GetRange(sr);
  }
  else
  {
    return;
  }   
   
  // set range
  m_ColorMappingLUT->SetTableRange(sr);
  m_SurfaceMapper->SetScalarRange(sr);
  m_SurfaceMapper->SetScalarVisibility(m_EnableMap);
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

  // select arrows or lines for glyphs
  if (m_GlyphType == GLYPH_LINES)
  {
    m_Glyph->SetSourceConnection(m_Line->GetOutputPort());
  }
  else if (m_GlyphType == GLYPH_ARROWS)
  {
    m_Arrow->SetTipResolution(m_GlyphRes);
    m_Arrow->SetTipRadius(m_GlyphRadius);
    m_Arrow->SetTipLength(m_GlyphLength);
    m_Arrow->SetShaftResolution(m_GlyphRes);
    m_Arrow->SetShaftRadius(m_GlyphRadius*0.3);
    m_Arrow->Update();
    m_Glyph->SetSourceConnection(m_Arrow->GetOutputPort());
  }
 
  // compute new scaling value to view dimensions
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
    
  // change visualization type
  if (nOfComponents==3)
  {
		m_AttributeAssigner->Assign(GetVectorFieldName(m_VectorFieldIndex), vtkDataSetAttributes::VECTORS, m_DataType);
    m_Glyph->SetVectorModeToUseVector();
    m_Glyph->SetScaleModeToScaleByVector();
    m_Glyph->SetColorModeToColorByVector();
  }
  else
  {
		m_AttributeAssigner->Assign(GetScalarFieldName(m_ScalarFieldIndex), vtkDataSetAttributes::SCALARS, m_DataType);
    m_Glyph->SetVectorModeToUseNormal();
    m_Glyph->SetScaleModeToScaleByScalar();
    m_Glyph->SetColorModeToColorByScalar();
  }

  // update mapper
  m_GlyphMapper->SetScalarRange(m_SurfaceMapper->GetLookupTable()->GetRange());
  m_GlyphMapper->SetLookupTable(m_ColorMappingLUT);
  m_GlyphMapper->SetScalarVisibility(m_EnableMap);

  m_GlyphActor->SetVisibility(m_ShowGlyphs);

  m_Glyph->Update();
}
//------------------------------------------------------------------------
int albaPipeVectorFieldMapWithArrows::GetNumberOfFields(bool bVectors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  vtkCellData* cd = m_Vme->GetOutput()->GetVTKData()->GetCellData();

  int nRet = -1;
  int nCount = 0;

  // get number of fields from points. If found, exit. (NB: only points data fields will be displayed)
  if (pd != NULL) 
  {
    int nQueryComps = bVectors ? 3 : 1;
    nRet = 0;
    nCount = pd->GetNumberOfArrays();
    for (int i = 0; i < nCount; i++)
    {
      vtkDataArray* da = pd->GetArray(i);
      if (da->GetNumberOfComponents() == nQueryComps)
        nRet++; 
    }
    if (nRet>0) {
      // set here global data type flag
      m_DataType = POINT_DATA;
      return nRet;
    }
  }
  // if not found, get it from cells. (NB: only cell data fields will be displayed)
  if (cd != NULL)  
  {
    int nQueryComps = bVectors ? 3 : 1;
    nRet = 0;
    nCount = cd->GetNumberOfArrays();
    for (int i = 0; i < nCount; i++)
    {
      vtkDataArray* da = cd->GetArray(i);
      if (da->GetNumberOfComponents() == nQueryComps)
        nRet++;
    }
    if (nRet>0) {
      // set here global data type flag
      m_DataType = CELL_DATA;
      return nRet;
    }
  }

  return nRet;
}
//------------------------------------------------------------------------
const char* albaPipeVectorFieldMapWithArrows::GetFieldName(int nIndex, bool bVectors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  vtkCellData* cd = m_Vme->GetOutput()->GetVTKData()->GetCellData();
  
  switch (m_DataType)
  {
  // get field name from points
  case POINT_DATA:
    {
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
    }
  break;
  // get field name form cells
  case CELL_DATA:
    {
      if (cd == NULL)
        return NULL;

      int nQueryComps = bVectors ? 3 : 1;
      int nCount = cd->GetNumberOfArrays();
      for (int i = 0; i < nCount; i++)
      {
        vtkDataArray* da = cd->GetArray(i);
        if (da->GetNumberOfComponents() == nQueryComps)
        {
          if (nIndex == 0)
            return da->GetName();
          
          nIndex--;
        }
      }
    }
  break;
  }
  
  return NULL;
}

//------------------------------------------------------------------------
int albaPipeVectorFieldMapWithArrows::GetFieldIndex(const char* szName, bool bVectors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  vtkCellData* cd = m_Vme->GetOutput()->GetVTKData()->GetCellData();
  
  int nIndex = 0;
  
  switch (m_DataType)
  {
  // get field index from points
  case POINT_DATA:
    {
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

      int nCount = pd->GetNumberOfArrays();
      for (int i = 0; i < nCount; i++)
      {
        vtkDataArray* da = pd->GetArray(i);
        if (da == pQueryDA)
          break; 
        
        if (da->GetNumberOfComponents() == nQueryComps)
          nIndex++;
      }
    }
  break;
  // get field index from cells
  case CELL_DATA:
    {
      if (cd == NULL)
        return -1;

      int nQueryComps;
      vtkDataArray* pQueryDA;

      if (!bVectors)
      {
        nQueryComps = 1;
        pQueryDA = cd->GetScalars(szName);
      }
      else
      {
        nQueryComps = 3;
        pQueryDA = cd->GetVectors(szName);
      }
       
      if (pQueryDA == NULL)
        return -1;

      int nCount = cd->GetNumberOfArrays();
      for (int i = 0; i < nCount; i++)
      {
        vtkDataArray* da = cd->GetArray(i);
        if (da == pQueryDA)
          break;  
        
        if (da->GetNumberOfComponents() == nQueryComps)
          nIndex++; 
      }
    }
  break;
  }
  
  return nIndex;
}


//------------------------------------------------------------------------
void albaPipeVectorFieldMapWithArrows::PopulateCombo(wxComboBox* combo, bool bVectors)
//------------------------------------------------------------------------
{
  vtkPointData* pd = m_Vme->GetOutput()->GetVTKData()->GetPointData();
  vtkCellData* cd = m_Vme->GetOutput()->GetVTKData()->GetCellData();
  
  switch (m_DataType)
  {
  // populate combo with point field data
  case POINT_DATA:
    {
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
    break;
  // populate combo with cell field data
  case CELL_DATA:
    {
      if (pd != NULL)
      {
        int nQueryComps = bVectors ? 3 : 1;
        int nCount = cd->GetNumberOfArrays();
        for (int i = 0; i < nCount; i++)
        {
          vtkDataArray* da = cd->GetArray(i);
          if (da->GetNumberOfComponents() == nQueryComps){
            combo->Append(da->GetName());
          }
        }
      }
    }
    break;
  }
}