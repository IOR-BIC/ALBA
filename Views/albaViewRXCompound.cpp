/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewRXCompound
 Authors: Stefano Perticoni , Paolo Quadrani, Daniele Giunchi
 
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

#include "albaViewRXCompound.h"
#include "albaViewVTK.h"
#include "albaViewRX.h"
#include "albaViewSlice.h"
#include "albaPipeSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVMEIterator.h"
#include "albaGUILutPreset.h"
#include "albaGUI.h"
#include "albaGUILutSwatch.h"
#include "albaGUILutSlider.h"
#include "albaGizmoSlice.h"
#include "mmaVolumeMaterial.h"
#include "albaVMEVolume.h"
#include "albaVMESurface.h"

#include "vtkDataSet.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum RX_SUBVIEW_ID
{
  RX_FRONT_VIEW = 0,
  RX_SIDE_VIEW,
  VIEWS_NUMBER,
};


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewRXCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewRXCompound::albaViewRXCompound(wxString label)
: albaViewCompound(label, 1, 3)
//----------------------------------------------------------------------------
{

  m_LutWidget = NULL;
  m_CurrentVolume = NULL;
  m_LayoutConfiguration = LAYOUT_CUSTOM;

  m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
    
  m_LutSliders[RX_FRONT_VIEW] = m_LutSliders[RX_SIDE_VIEW] = NULL;
  m_VtkLUT[RX_FRONT_VIEW] = m_VtkLUT[RX_SIDE_VIEW] = NULL ;

  m_RightOrLeft=1;
  m_CurrentSurface.clear();

}
//----------------------------------------------------------------------------
albaViewRXCompound::~albaViewRXCompound()
//----------------------------------------------------------------------------
{
  m_ViewsRX[RX_FRONT_VIEW] = m_ViewsRX[RX_SIDE_VIEW] = NULL;
  
  m_CurrentSurface.clear();
  
  for (int i = RX_FRONT_VIEW;i < VIEWS_NUMBER;i++)
  {
    cppDEL(m_LutSliders[i]);
    vtkDEL(m_VtkLUT[i]);
  }
}
//----------------------------------------------------------------------------
albaView *albaViewRXCompound::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewRXCompound *v = new albaViewRXCompound(m_Label);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  for (int i=0;i<m_PluggedChildViewList.size();i++)
  {
    v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
  }
  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaViewRXCompound::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<VIEWS_NUMBER; i++)
    ((albaViewRX *)m_ChildViewList[i])->VmeShow(vme, show);

  if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
  {
    if (show)
    {
         
      // set the range for every slider widget
      for (int childID = RX_FRONT_VIEW; childID < VIEWS_NUMBER; childID++)
      {
        double minMax[2];
        ((albaViewRX *)(m_ChildViewList[childID]))->GetLutRange(minMax);

        m_LutSliders[childID]->SetRange(minMax[0],minMax[1]);
        m_LutSliders[childID]->SetSubRange(minMax[0],minMax[1]);
      
        // create a lookup table for each RX view
        vtkNEW(m_VtkLUT[childID]);
        m_VtkLUT[childID]->SetRange(minMax);
        m_VtkLUT[childID]->Build();
        lutPreset(4,m_VtkLUT[childID]);
        m_VtkLUT[childID]->SetRange(minMax);
        m_VtkLUT[childID]->Build();
        lutPreset(4,m_VtkLUT[childID]);

        ((albaViewRX *)m_ChildViewList[childID])->SetLutRange(minMax[0],minMax[1]);

      }
      
      m_CurrentVolume = vme;

    }
    else
    {
      
      m_CurrentVolume = NULL;
      
    }
  }
    
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void albaViewRXCompound::VmeRemove(albaVME *vme)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && vme == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
    
  }
  Superclass::VmeRemove(vme);
}
//----------------------------------------------------------------------------
void albaViewRXCompound::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(alba_event->GetId()) 
    { 
      // events from the slider
      case ID_RANGE_MODIFIED:
      {
        // is the volume visible?
        if(((albaViewSlice *)m_ChildViewList[RX_FRONT_VIEW])->VolumeIsVisible())
        {
          double low, hi;

          // from which lut slider the event is coming?
          if (alba_event->GetSender() == m_LutSliders[RX_FRONT_VIEW])
          {
            m_LutSliders[RX_FRONT_VIEW]->GetSubRange(&low,&hi);
            ((albaViewRX *)m_ChildViewList[RX_FRONT_VIEW])->SetLutRange(low,hi);
          }
          else if (alba_event->GetSender() == m_LutSliders[RX_SIDE_VIEW])
          {
            m_LutSliders[RX_SIDE_VIEW]->GetSubRange(&low,&hi);
            ((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->SetLutRange(low,hi);
          }
          

          CameraUpdate();
        }
      }
      break;
      case ID_RIGHT_OR_LEFT:
        {
          if (m_RightOrLeft==0)
          {
            ((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_RIGHT);
          }
          else
            ((albaViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_LEFT);
        }
      break;
      default:
        albaViewCompound::OnEvent(alba_event);
    }
  }
  else
  {
    albaViewCompound::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
albaGUI* albaViewRXCompound::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();
  
	wxString povChoices[2]{ "Right","Left" };
	m_Gui->Radio(ID_RIGHT_OR_LEFT, "POV", &m_RightOrLeft, 2, povChoices, 2, "Point Of View");

  m_Gui->Divider(1);
  
  EnableWidgets(m_CurrentVolume != NULL);

	for(int i=RX_FRONT_VIEW;i<=RX_SIDE_VIEW;i++)
		((albaViewRX*)m_ChildViewList[i])->GetGui();

	m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewRXCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new albaGUI(this);
  wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

  // create three windowing widgets
  for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
  {
    m_LutSliders[i] = new albaGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
    //EnableWidgets(m_CurrentVolume != NULL);
    m_LutSliders[i]->SetListener(this);
    m_LutSliders[i]->SetMinSize(wxSize(500,24));
    lutsSizer->Add(m_LutSliders[i],wxALIGN_CENTER|wxRIGHT);
  }

  m_GuiView->Add(lutsSizer);
  m_GuiView->Reparent(m_Win);
  m_GuiView->FitGui();
  m_GuiView->Update();
}

//----------------------------------------------------------------------------
void albaViewRXCompound::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[2] = {CAMERA_RX_FRONT, CAMERA_RX_LEFT};
  for(int v=RX_FRONT_VIEW; v<VIEWS_NUMBER; v++)
  {
    // create to the child view
    m_ViewsRX[v] = new albaViewRX("RX child view", cam_pos[v]);
    m_ViewsRX[v]->PlugVisualPipe("albaVMEVolumeGray", "albaPipeVolumeProjected",MUTEX);
    m_ViewsRX[v]->PlugVisualPipe("albaVMELabeledVolume", "albaPipeVolumeProjected",MUTEX);
    
    PlugChildView(m_ViewsRX[v]);
  }
  
}
//----------------------------------------------------------------------------
void albaViewRXCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_LUT_WIDGET,enable);
  }
}
//----------------------------------------------------------------------------
void albaViewRXCompound::LayoutSubView(int width, int height)
//----------------------------------------------------------------------------
{
  // this implement the Fixed SubViews Layout
  int border = 2;
  int x_pos, c, i;

  int step_width  = (width-border) / 2;
  i = 0;
  for (c = 0; c < m_NumOfChildView; c++)
  {
    x_pos = c*(step_width + border);
    m_ChildViewList[i]->GetWindow()->SetSize(x_pos, 0, step_width, height);
    i++;
  }
  
}
