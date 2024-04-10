/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeVectorFieldSlice.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-06-09 16:04:32 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaPipeVectorFieldSlice.h"

#include "albaSceneNode.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaGUIFloatSlider.h"
#include "albaVME.h"

#include "vtkDataSet.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataArray.h"

#include "wx/busyinfo.h"
#include "wx/statline.h"
#include <float.h>


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVectorFieldSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeVectorFieldSlice::albaPipeVectorFieldSlice() : albaPipeVectorFieldSurface()
//----------------------------------------------------------------------------
{
  m_SliceDirection = DIR_Z;    
  m_SlicePos = 0.0;

  m_TextSlice = NULL;
  m_SliderSlice = NULL;
  m_CutPlane = NULL;
}

//----------------------------------------------------------------------------
albaPipeVectorFieldSlice::~albaPipeVectorFieldSlice()
//----------------------------------------------------------------------------
{  
  vtkDEL(m_CutPlane);
 
}


//----------------------------------------------------------------------------
albaGUI *albaPipeVectorFieldSlice::CreateGui()
//----------------------------------------------------------------------------
{
  Superclass::CreateGui();
       
  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );  
  bSizerMain->Add( new wxStaticLine( m_Gui, wxID_ANY, wxDefaultPosition, 
    wxDefaultSize, wxLI_HORIZONTAL ), 0, wxEXPAND | wxALL, 5 );

  wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
  bSizer1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Slice Dir.:"), 
    wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

  wxComboBox* comboSliceDir = new wxComboBox( m_Gui, ID_SLICE_DIRECTION, 
    wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
  comboSliceDir->Append( _("X axis") );
  comboSliceDir->Append( _("Y axis") );
  comboSliceDir->Append( _("Z axis") );
  comboSliceDir->SetToolTip( _("Selects the direction of the cutting plane.") );
  bSizer1->Add( comboSliceDir, 1, wxALL, 1 );
  bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );
  
  //and validator
  comboSliceDir->SetValidator(albaGUIValidator(this, ID_SLICE_DIRECTION, comboSliceDir, &m_SliceDirection));
  

  wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
  bSizer2->Add( new wxStaticText( m_Gui, wxID_ANY, _("Slice Pos.:"), 
    wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

  m_SliderSlice = new albaGUIFloatSlider( m_Gui, ID_SLICE_POS_SLIDER, m_SlicePos);
  bSizer2->Add( m_SliderSlice, 1, wxALL, 0 );

  m_TextSlice = new wxTextCtrl( m_Gui, ID_SLICE_POS, wxEmptyString, 
    wxDefaultPosition, wxSize( 40,-1 ), 0 );
  bSizer2->Add( m_TextSlice, 0, wxALL, 0 );
  bSizerMain->Add( bSizer2, 0, wxEXPAND, 5 );  
  
  m_Gui->Add(bSizerMain);
  UpdateSlicePosRange();
  return m_Gui;
}


//----------------------------------------------------------------------------
void albaPipeVectorFieldSlice::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{			
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
      if (e->GetId() == ID_SLICE_DIRECTION) {
        UpdateSlicePosRange();
      }

      UpdateVTKPipe(); 
    
			GetLogicManager()->CameraUpdate();
      return;
    }     
  }

  //let the ancestor deal with the event
  Superclass::OnEvent(alba_event);
}

//------------------------------------------------------------------------
//Updates the range of slice position.
//If the current slice position is outside the range, it is changed. 
//Sets new validators for slice position widgets.
/*virtual*/ void albaPipeVectorFieldSlice::UpdateSlicePosRange()
//------------------------------------------------------------------------
{
  double bounds[6]; 
  m_Vme->GetOutput()->GetVTKData()->GetBounds(bounds);
  
  double* range = &bounds[0] + (m_SliceDirection - DIR_X)*2;
  

  m_TextSlice->SetValidator(albaGUIValidator(this, ID_SLICE_POS, m_TextSlice, 
    &m_SlicePos, m_SliderSlice, range[0],  range[1]));
  m_SliderSlice->SetValidator(albaGUIValidator(this, ID_SLICE_POS, 
    m_SliderSlice, &m_SlicePos, m_TextSlice));
  m_SliderSlice->SetRange(range[0],  range[1]); 
}

//------------------------------------------------------------------------
//Constructs VTK pipeline.
/*virtual*/ void albaPipeVectorFieldSlice::CreateVTKPipe()
//------------------------------------------------------------------------
{  
  Superclass::CreateVTKPipe();

  //create cutter
  vtkNEW(m_CutPlane);

  vtkCutter* cutter = vtkCutter::New();
  cutter->SetInputData(m_Vme->GetOutput()->GetVTKData());
  cutter->SetCutFunction(m_CutPlane);

  m_SurfaceMapper->SetInputConnection(cutter->GetOutputPort());
  cutter->Delete(); //no longer needed
}

//------------------------------------------------------------------------
//Updates VTK pipeline (setting radius, etc.). 
/*virtual*/ void albaPipeVectorFieldSlice::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  //update cutter  
  double bounds[6];
  m_Vme->GetOutput()->GetVTKData()->GetBounds(bounds);

  double origin[3];
  for (int i = 0, j = 0; i < 3; i++, j += 2){
    origin[i] = (bounds[j] + bounds[j + 1])*0.5;
  }

  origin[m_SliceDirection - DIR_X] = m_SlicePos;
  
  double normal[3] = {0.0, 0.0, 0.0};
  normal[m_SliceDirection - DIR_X] = 1.0;

  m_CutPlane->SetOrigin(origin);
  m_CutPlane->SetNormal(normal);
  
  Superclass::UpdateVTKPipe();
}