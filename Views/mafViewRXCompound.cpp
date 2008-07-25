/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXCompound.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:25:11 $
  Version:   $Revision: 1.8 $
  Authors:   Stefano Perticoni , Paolo Quadrani, Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewRXCompound.h"
#include "mafViewVTK.h"
#include "mafViewRX.h"
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafNodeIterator.h"
#include "mafGUILutPreset.h"
#include "mafGUI.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutSlider.h"
#include "mafGizmoSlice.h"
#include "mmaVolumeMaterial.h"
#include "mafVMEVolume.h"
#include "mafVMESurface.h"

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
mafCxxTypeMacro(mafViewRXCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewRXCompound::mafViewRXCompound(wxString label)
: mafViewCompound(label, 1, 3)
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
mafViewRXCompound::~mafViewRXCompound()
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
mafView *mafViewRXCompound::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewRXCompound *v = new mafViewRXCompound(m_Label);
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
void mafViewRXCompound::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<VIEWS_NUMBER; i++)
    ((mafViewRX *)m_ChildViewList[i])->VmeShow(node, show);

  if (((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (show)
    {
         
      // set the range for every slider widget
      for (int childID = RX_FRONT_VIEW; childID < VIEWS_NUMBER; childID++)
      {
        double minMax[2];
        ((mafViewRX *)(m_ChildViewList[childID]))->GetLutRange(minMax);

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

        ((mafViewRX *)m_ChildViewList[childID])->SetLutRange(minMax[0],minMax[1]);

      }
      
      m_CurrentVolume = mafVME::SafeDownCast(node);

    }
    else
    {
      
      m_CurrentVolume = NULL;
      
    }
  }
    
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewRXCompound::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && node == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
    
  }
  Superclass::VmeRemove(node);
}
//----------------------------------------------------------------------------
void mafViewRXCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(maf_event->GetId()) 
    { 
      // events from the slider
      case ID_RANGE_MODIFIED:
      {
        // is the volume visible?
        if(((mafViewSlice *)m_ChildViewList[RX_FRONT_VIEW])->VolumeIsVisible())
        {
          double low, hi;

          // from which lut slider the event is coming?
          if (maf_event->GetSender() == m_LutSliders[RX_FRONT_VIEW])
          {
            m_LutSliders[RX_FRONT_VIEW]->GetSubRange(&low,&hi);
            ((mafViewRX *)m_ChildViewList[RX_FRONT_VIEW])->SetLutRange(low,hi);
          }
          else if (maf_event->GetSender() == m_LutSliders[RX_SIDE_VIEW])
          {
            m_LutSliders[RX_SIDE_VIEW]->GetSubRange(&low,&hi);
            ((mafViewRX *)m_ChildViewList[RX_SIDE_VIEW])->SetLutRange(low,hi);
          }
          

          CameraUpdate();
        }
      }
      break;
      case ID_RIGHT_OR_LEFT:
        {
          if (m_RightOrLeft==0)
          {
            ((mafViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_RIGHT);
          }
          else
            ((mafViewRX *)m_ChildViewList[RX_SIDE_VIEW])->CameraSet(CAMERA_RX_LEFT);
        }
      break;
      default:
        mafViewCompound::OnEvent(maf_event);
    }
  }
  else
  {
    mafViewCompound::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
mafGUI* mafViewRXCompound::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  
  wxString m_Choices[2];
  m_Choices[0]="Right";
  m_Choices[1]="Left";
  m_Gui->Radio(ID_RIGHT_OR_LEFT,"Side",&m_RightOrLeft,2,m_Choices);

  m_Gui->Divider(1);
  
  EnableWidgets(m_CurrentVolume != NULL);

	for(int i=RX_FRONT_VIEW;i<=RX_SIDE_VIEW;i++)
		((mafViewRX*)m_ChildViewList[i])->GetGui();

	m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewRXCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mafGUI(this);
  wxBoxSizer *lutsSizer = new wxBoxSizer(wxHORIZONTAL);

  // create three windowing widgets
  for (int i = RX_FRONT_VIEW; i < VIEWS_NUMBER; i++)
  {
    m_LutSliders[i] = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
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
void mafViewRXCompound::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[2] = {CAMERA_RX_FRONT, CAMERA_RX_LEFT};
  for(int v=RX_FRONT_VIEW; v<VIEWS_NUMBER; v++)
  {
    // create to the child view
    m_ViewsRX[v] = new mafViewRX("RX child view", cam_pos[v]);
    m_ViewsRX[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeProjected",MUTEX);
    m_ViewsRX[v]->PlugVisualPipe("mafVMELabeledVolume", "mafPipeVolumeProjected",MUTEX);
    
    PlugChildView(m_ViewsRX[v]);
  }
  
}
//----------------------------------------------------------------------------
void mafViewRXCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_LUT_WIDGET,enable);
  }
}
//----------------------------------------------------------------------------
void mafViewRXCompound::LayoutSubViewCustom(int width, int height)
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
