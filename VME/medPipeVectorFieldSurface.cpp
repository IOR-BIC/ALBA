/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeVectorFieldSurface.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-06-09 16:04:32 $ 
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
#include "medPipeVectorFieldSurface.h"

#include "mafSceneNode.h"
#include "mafPipeFactory.h"
#include "mafGUI.h"
#include "mafGUIValidator.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutPreset.h"
#include "mafVME.h"

#include "vtkMAFAssembly.h"
#include "vtkDataSet.h"
#include "vtkGeometryFilter.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkLookupTable.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkScalarBarActor.h"
#include "vtkRenderer.h"

//#include "vtkArrayCalculator.h"

#include "wx/busyinfo.h"
#include <float.h>


//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeVectorFieldSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeVectorFieldSurface::medPipeVectorFieldSurface() : medPipeVectorField()
//----------------------------------------------------------------------------
{     
  m_ColorMappingLUT = NULL;  
  m_ColorMappingMode = 0;  
  m_ShowMapping = 1;  
    
  m_SurfaceActor = NULL;
  m_SurfaceMapper = NULL;
  
  m_MappingActor = NULL; 
}

//----------------------------------------------------------------------------
medPipeVectorFieldSurface::~medPipeVectorFieldSurface()
//----------------------------------------------------------------------------
{  
  m_AssemblyFront->RemovePart(m_SurfaceActor);  
  m_RenFront->RemoveActor2D(m_MappingActor);  
  
  vtkDEL(m_MappingActor);
  
  vtkDEL(m_SurfaceActor);
  vtkDEL(m_SurfaceMapper);
  
  vtkDEL(m_ColorMappingLUT);  
}


//----------------------------------------------------------------------------
mafGUI *medPipeVectorFieldSurface::CreateGui()
//----------------------------------------------------------------------------
{
  int nVectors = GetNumberOfVectors();
  
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

      wxComboBox* comboField = new wxComboBox( m_Gui, ID_VECTORFIELD, wxEmptyString, 
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboField->SetToolTip( _("Selects the vector field to be visualized.") );
      PopulateCombo(comboField, true);  //at least one vector is available
      bSizer1->Add( comboField, 1, wxALL, 1 );
      bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );

      //and validator
      comboField->SetValidator(mafGUIValidator(this, ID_VECTORFIELD, comboField, &m_VectorFieldIndex));
    }

    
    wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
    bSizer2->Add( new wxStaticText( m_Gui, wxID_ANY, _("Color by:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxComboBox* comboColorBy = new wxComboBox( m_Gui, ID_COLOR_MAPPING_MODE, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    comboColorBy->Append( _("magnitude") );
    comboColorBy->Append( _("X component") );
    comboColorBy->Append( _("Y component") );
    comboColorBy->Append( _("Z component") );
    comboColorBy->SetToolTip( 
      _("Specifies how the specified vector field should be transformed into colors.") );
    bSizer2->Add( comboColorBy, 1, wxALL, 0 );
    bSizerMain->Add( bSizer2, 0, wxEXPAND, 5 );

    //and validator
    comboColorBy->SetValidator(mafGUIValidator(this, ID_COLOR_MAPPING_MODE, comboColorBy, &m_ColorMappingMode));
  
    wxBoxSizer* bSizer3 = new wxBoxSizer( wxHORIZONTAL );    
    bSizer3->Add( new wxStaticText( m_Gui, wxID_ANY, _("LUT:")), 0, wxALL, 5 );

    mafGUILutSwatch* luts2 = new mafGUILutSwatch(m_Gui, ID_COLOR_MAPPING_LUT, 
      wxDefaultPosition, wxSize(140, 18), wxTAB_TRAVERSAL | wxSIMPLE_BORDER );
    luts2->SetLut(m_ColorMappingLUT);
    luts2->SetEditable(true);
    luts2->SetListener(this);
    bSizer3->Add(luts2, 0, 0/*wxEXPAND*/, 5 );
    bSizerMain->Add( bSizer3, 1, wxEXPAND, 5 );

    wxCheckBox* chckShowSFLegend1 = new wxCheckBox( m_Gui, ID_SHOW_COLOR_MAPPING, 
      _("Show Color Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
    chckShowSFLegend1->SetToolTip( _("If checked, mapping is displayed "
      "as a colored bar in the main view.") );
    bSizerMain->Add( chckShowSFLegend1, 0, wxALL|wxEXPAND, 5 );

    //and validator
    chckShowSFLegend1->SetValidator(mafGUIValidator(this, ID_SHOW_COLOR_MAPPING,       
      chckShowSFLegend1, &m_ShowMapping));
  }

  m_Gui->Add(bSizerMain);
  return m_Gui;
}


//----------------------------------------------------------------------------
void medPipeVectorFieldSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{			
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
      UpdateVTKPipe(); 
    
      mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      return;
    }     
  }

  //forward it to our listener to deal with it
  mafEventMacro(*maf_event);
}

//------------------------------------------------------------------------
//Constructs VTK pipeline.
/*virtual*/ void medPipeVectorFieldSurface::CreateVTKPipe()
//------------------------------------------------------------------------
{  
  //build LUT
  m_ColorMappingLUT = vtkLookupTable::New();
  lutPreset(12, m_ColorMappingLUT); //initialize LUT to SAR (it has index 12)  
  m_ColorMappingLUT->Build(); 

  vtkGeometryFilter* filter = vtkGeometryFilter::New();
  filter->SetInput(m_Vme->GetOutput()->GetVTKData());

  m_SurfaceMapper = vtkPolyDataMapper::New();
  m_SurfaceMapper->SetInput(filter->GetOutput());
  m_SurfaceMapper->ImmediateModeRenderingOn();
  m_SurfaceMapper->SetScalarModeToUsePointFieldData();// PointData();
  m_SurfaceMapper->SetColorModeToMapScalars();
  m_SurfaceMapper->SetLookupTable(m_ColorMappingLUT);

  filter->Delete(); //no longer needed

  m_SurfaceActor = vtkActor::New();
  m_SurfaceActor->SetMapper(m_SurfaceMapper);
  m_SurfaceActor->SetPickable(0);   //make it faster

  //scalar field map
  m_MappingActor = vtkScalarBarActor::New();
  m_MappingActor->SetLookupTable(m_SurfaceMapper->GetLookupTable());
  ((vtkActor2D*)m_MappingActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  ((vtkActor2D*)m_MappingActor)->GetPositionCoordinate()->SetValue(0.1,0.01);
  m_MappingActor->SetOrientationToHorizontal();
  m_MappingActor->SetWidth(0.8);
  m_MappingActor->SetHeight(0.12);  
  m_MappingActor->SetLabelFormat("%6.3g");
  m_MappingActor->SetPickable(0);   //make it faster

  m_RenFront->AddActor2D(m_MappingActor);
  m_AssemblyFront->AddPart(m_SurfaceActor); 
}

//------------------------------------------------------------------------
//Updates VTK pipeline (setting radius, etc.). 
/*virtual*/ void medPipeVectorFieldSurface::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  const char* vector_name = GetVectorFieldName(m_VectorFieldIndex);
  m_SurfaceMapper->SelectColorArray(vector_name);

  vtkDataArray* da = m_Vme->GetOutput()->GetVTKData()->
    GetPointData()->GetVectors(vector_name);

  double sr[2];
  if (m_ColorMappingMode != CMM_MAGNITUDE)  
  {
    //X, Y or Z component
    m_ColorMappingLUT->SetVectorModeToComponent();
    m_ColorMappingLUT->SetVectorComponent(m_ColorMappingMode - CMM_X);
    da->GetRange(sr, m_ColorMappingMode - CMM_X);
  }
  else
  {
    //magnitude
    m_ColorMappingLUT->SetVectorModeToMagnitude();

    //we need to compute scalar range    
    int nCount = da->GetNumberOfTuples();
    const double* vector = da->GetTuple(0);
    sr[0] = sr[1] = vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2];
    for (int i = 1; i < nCount; i++)
    {
      vector = da->GetTuple(i);
      double mag = vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2];
      if (mag < sr[0])
        sr[0] = mag;

      if (mag > sr[1])
        sr[1] = mag;
    }

    sr[0] = sqrt(sr[0]);
    sr[1] = sqrt(sr[1]);
  }

  m_ColorMappingLUT->SetTableRange(sr);
  m_SurfaceMapper->SetScalarRange(sr);
  m_SurfaceMapper->Update();  
  
//  m_MappingActor->SetTitle(scalar_name);
  m_MappingActor->SetVisibility(m_ShowMapping);  
}