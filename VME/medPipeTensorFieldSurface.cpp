/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeTensorFieldSurface.cpp,v $ 
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
#include "medPipeTensorFieldSurface.h"

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
mafCxxTypeMacro(medPipeTensorFieldSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeTensorFieldSurface::medPipeTensorFieldSurface() : medPipeTensorField()
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
medPipeTensorFieldSurface::~medPipeTensorFieldSurface()
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
mafGUI *medPipeTensorFieldSurface::CreateGui()
//----------------------------------------------------------------------------
{
  int nTensors = GetNumberOfTensors();
  
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
        wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboField = new wxComboBox( m_Gui, ID_VECTORFIELD, wxEmptyString, 
        wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboField->SetToolTip( _("Selects the tensor field to be visualized.") );
      PopulateCombo(comboField, true);  //at least one tensor is available
      bSizer1->Add( comboField, 1, wxALL, 1 );
      bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );

      //and validator
      comboField->SetValidator(mafGUIValidator(this, ID_VECTORFIELD, comboField, &m_TensorFieldIndex));
    }
    
    wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
    bSizer2->Add( new wxStaticText( m_Gui, wxID_ANY, _("Color by:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_comboColorBy = new wxComboBox( m_Gui, ID_COLOR_MAPPING_MODE, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    UpdateColorByCombo();      
    
    m_comboColorBy->SetToolTip( 
      _("Specifies how the specified tensor field should be transformed into colors.") );
    bSizer2->Add( m_comboColorBy, 1, wxALL, 0 );
    bSizerMain->Add( bSizer2, 0, wxEXPAND, 5 );

    //and validator
    m_comboColorBy->SetValidator(mafGUIValidator(this, ID_COLOR_MAPPING_MODE, m_comboColorBy, &m_ColorMappingMode));
  
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
void medPipeTensorFieldSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{			
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
      if (e->GetId() == ID_VECTORFIELD)
        UpdateColorByCombo(); //we need to update list of components

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
/*virtual*/ void medPipeTensorFieldSurface::CreateVTKPipe()
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
/*virtual*/ void medPipeTensorFieldSurface::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  const char* tensor_name = GetTensorFieldName(m_TensorFieldIndex);
  m_SurfaceMapper->SelectColorArray(tensor_name);

  vtkDataArray* da = m_Vme->GetOutput()->GetVTKData()->
    GetPointData()->GetTensors(tensor_name);

  int idx = m_ColorMappingMode - CMM_COMPONENT1;  //index of component

  double sr[2];
  if (m_ColorMappingMode == CMM_MAGNITUDE)      
    m_ColorMappingLUT->SetVectorModeToMagnitude();    //magnitude
  else
  {
    //some component
    m_ColorMappingLUT->SetVectorModeToComponent();
    m_ColorMappingLUT->SetVectorComponent(idx);    
  }

  //get range for the given component / magnitude
  //RELASE NOTE: GetRange has an undocumented feature to compute
  //magnitude, if the component parameter is negative
  //BUT it has also a BUG, it cannot support more than 3 components
  if (idx < CMM_COMPONENT3)
    da->GetRange(sr, idx);
  else
  {    
    //we need to compute scalar range (because of a BUG in vtkDataArray)
    sr[0] = DBL_MAX; sr[1] = -DBL_MAX;
    
    int nCount = da->GetNumberOfTuples();
    for (int i = 0; i < nCount; i++)
    {
      double value = da->GetComponent(i, idx);
      if (value < sr[0])
        sr[0] = value;

      if (value > sr[1])
        sr[1] = value;
    }
  }

  m_ColorMappingLUT->SetTableRange(sr);
  m_SurfaceMapper->SetScalarRange(sr);
  m_SurfaceMapper->Update();  
  
//  m_MappingActor->SetTitle(scalar_name);
  m_MappingActor->SetVisibility(m_ShowMapping);  
}

//------------------------------------------------------------------------
//Updates the content of m_comboColorBy combobox
//"magnitude" and 0..NumberOfComponents-1 will be listed.
/*virtual*/ void medPipeTensorFieldSurface::UpdateColorByCombo()
//------------------------------------------------------------------------
{
  int nOldColorMode = m_ColorMappingMode;

  m_comboColorBy->Clear();
  m_comboColorBy->Append( _("magnitude") );

  if (m_TensorFieldIndex >= 0)
  {
    vtkDataArray* da = m_Vme->GetOutput()->GetVTKData()->
      GetPointData()->GetTensors(GetTensorFieldName(m_TensorFieldIndex));

    if (da != NULL)
    {
      int nComps = da->GetNumberOfComponents();
      for (int i = 0; i < nComps; i++){
        m_comboColorBy->Append(wxString::Format("%d", i));
      }
    }
  } //end if [valid selection of tensor field]

  int nCount = m_comboColorBy->GetCount();
  if (nOldColorMode >= nCount)
    nOldColorMode = 0;

  m_ColorMappingMode = nOldColorMode;
  m_comboColorBy->SetSelection(m_ColorMappingMode);
}