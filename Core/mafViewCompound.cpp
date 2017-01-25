/*=========================================================================

 Program: MAF2
 Module: mafViewCompound
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewCompound.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafInteractor.h"
#include "mafAvatar.h"
#include "mafAvatar3D.h"
#include "mafDeviceButtonsPadTracker.h"
#include "mafDeviceButtonsPadMouse.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewCompound::mafViewCompound(const wxString &label, int num_row, int num_col)
: mafView(label)
//----------------------------------------------------------------------------
{
  m_ViewRowNum = num_row;
  m_ViewColNum = num_col;
  m_NumOfPluggedChildren  = 0;
  m_DefauldChildView      = 0;
  m_NumOfChildView        = 0;
  m_LinkSubView           = 0;
  m_ChildViewList.clear();
  m_PluggedChildViewList.clear();
  m_GuiView = NULL;
  m_GuiViewWindow = NULL;
  m_SubViewMaximized = -1;
  m_Mouse = NULL;
  m_LayoutConfiguration = GRID_LAYOUT;
}
//----------------------------------------------------------------------------
mafViewCompound::~mafViewCompound()
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfPluggedChildren; i++)
  {
    cppDEL(m_PluggedChildViewList[i]);
  }
  for (int cv = 0; cv<m_NumOfChildView; cv++)
  {
    cppDEL(m_ChildViewList[cv]);
  }
  m_PluggedChildViewList.clear();
  m_ChildViewList.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewCompound::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewCompound *v = new mafViewCompound(m_Label, m_ViewRowNum, m_ViewColNum);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  for (int i=0;i<m_PluggedChildViewList.size();i++)
  {
    v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this,m_LightCopyEnabled));
  }
  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewCompound::PlugChildView(mafView *child)
//----------------------------------------------------------------------------
{
  if (child)
  {
    m_PluggedChildViewList.push_back(child);
    m_NumOfPluggedChildren++;
  }
}
//----------------------------------------------------------------------------
void mafViewCompound::Create()
//----------------------------------------------------------------------------
{
  mafGUIViewWin *w = new mafGUIViewWin(mafGetFrame(),-1);
  w->SetBackgroundColour(wxColour(102,102,102));
  w->m_Owner = this;
  w->Show(false);
  m_Win = w;

  for(int i=0; i<m_NumOfPluggedChildren; i++)
  {
    m_ChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this, m_LightCopyEnabled));
    if(m_ChildViewList[i]->GetWindow()) {//MOD_DAN_LIGHT
      m_ChildViewList[i]->GetWindow()->Reparent(m_Win);
      m_ChildViewList[i]->GetWindow()->Show(true);
    }
  }
  for (int f=m_NumOfPluggedChildren; f < m_ViewColNum * m_ViewRowNum; f++)
  {
    m_ChildViewList.push_back(m_PluggedChildViewList[m_NumOfPluggedChildren-1]->Copy(this, m_LightCopyEnabled));
    if(m_ChildViewList[f]->GetWindow()) {//MOD_DAN_LIGHT
        m_ChildViewList[f]->GetWindow()->Reparent(m_Win);
        m_ChildViewList[f]->GetWindow()->Show(true);
    }
  }
  m_NumOfChildView = m_ChildViewList.size();

  CreateGuiView();
}
//----------------------------------------------------------------------------
void mafViewCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
  /*m_GuiView = new mafGUI(this);
  m_GuiView->Label("Compound View's GUI",true);
  m_GuiView->Reparent(m_Win);*/
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeAdd(mafVME *vme)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeRemove(mafVME *vme)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeSelect(mafVME *vme, bool select)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeSelect(vme, select);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeShow(vme, show);
}
//----------------------------------------------------------------------------
int mafViewCompound::GetNodeStatus(mafVME *vme)
//----------------------------------------------------------------------------
{
  // should be redefined for compounded views
  return m_ChildViewList[m_DefauldChildView]->GetNodeStatus(vme);
}

//----------------------------------------------------------------------------
bool mafViewCompound::IsVmeShowed(mafVME *vme)
{
	return m_ChildViewList[m_DefauldChildView]->GetNodePipe(vme) != NULL;
}

//----------------------------------------------------------------------------
void mafViewCompound::CameraReset(mafVME *vme)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->CameraReset(vme);
}
//----------------------------------------------------------------------------
void mafViewCompound::CameraUpdate()
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->CameraUpdate();
}
//----------------------------------------------------------------------------
mafSceneGraph *mafViewCompound::GetSceneGraph()
//----------------------------------------------------------------------------
{
  //return ((mafViewVTK *)m_ChildViewList[m_DefauldChildView])->GetSceneGraph();
  return GetSubView()->GetSceneGraph();
}
//----------------------------------------------------------------------------
mafRWIBase *mafViewCompound::GetRWI()
//----------------------------------------------------------------------------
{
  //return ((mafViewVTK *)m_ChildViewList[m_DefauldChildView])->GetRWI();
  return GetSubView()->GetRWI();
}
//----------------------------------------------------------------------------
bool mafViewCompound::FindPokedVme(mafDevice *device,mafMatrix &point_pose,vtkProp3D *&picked_prop,mafVME *&picked_vme,mafInteractor *&picked_behavior)
//----------------------------------------------------------------------------
{
  mafViewVTK *v = mafViewVTK::SafeDownCast(GetSubView());
  if (v)
    return v->FindPokedVme(device,point_pose,picked_prop,picked_vme,picked_behavior);
  else
    return false;
}
//----------------------------------------------------------------------------
void mafViewCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
    case ID_DEFAULT_CHILD_VIEW:
    {
      mafSceneGraph *sg = GetSceneGraph();
      if(sg)
      {
        mafVME *vme = sg->GetSelectedVme();
        if (vme)
        {
					GetLogicManager()->VmeModified(vme);
        }
      }
    }
  	break;
    case ID_LAYOUT_CHOOSER:
      OnLayout();
    break;
    case ID_LINK_SUBVIEW:
      LinkView(m_LinkSubView != 0);
    break;
    default:
      mafView::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
mafGUI* mafViewCompound::CreateGui()
//-------------------------------------------------------------------------
{
  mafString childview_tooltip;
  childview_tooltip = "set the default child view";

  wxString layout_choices[4] = {"default","layout 1","layout 2", "custom"};

  assert(m_Gui == NULL);
	m_Gui = mafView::CreateGui();

  m_Gui->Integer(ID_DEFAULT_CHILD_VIEW,"default child", &m_DefauldChildView, 0, m_NumOfChildView, childview_tooltip);
  m_Gui->Combo(ID_LAYOUT_CHOOSER,"layout",&m_LayoutConfiguration,4,layout_choices);
  m_Gui->Bool(ID_LINK_SUBVIEW,"link camera",&m_LinkSubView);
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewCompound::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  m_Size = event.GetSize();
  OnLayout();
}
//----------------------------------------------------------------------------
void mafViewCompound::OnLayout()
//----------------------------------------------------------------------------
{
  assert(m_NumOfPluggedChildren == m_PluggedChildViewList.size());
  if (m_NumOfPluggedChildren == 0)
  {
    return;
  }

  int sw = m_Size.GetWidth();
  int sh = m_Size.GetHeight();
  int gh = 0;

  if (m_GuiViewWindow)
  {
    wxSize gui_size = m_GuiViewWindow->GetBestSize();
    gh = gui_size.GetHeight();
    if(sw<gh || sh<gh) return;
    sh -= gh;
    m_GuiViewWindow->SetMinSize(wxSize(sw,gh));
    m_GuiViewWindow->SetSize(0,sh,sw,gh);
  }

  if (m_GuiView)
  {
    wxSize gui_size = m_GuiView->GetBestSize();
    gh = gui_size.GetHeight();
    if(sw<gh || sh<gh) return;
    sh -= gh;
    m_GuiView->SetMinSize(wxSize(sw,gh));
    m_GuiView->SetSize(0,sh,sw,gh);
  }

  if (m_SubViewMaximized == -1)
  {
    LayoutSubView(sw,sh);
  }
  else
  {
    int i = 0;
    for (int r=0; r<m_ViewRowNum; r++)
    {
      for (int c=0; c<m_ViewColNum; c++)
      {
        if (i == m_SubViewMaximized)
        {
          m_ChildViewList[m_SubViewMaximized]->GetWindow()->SetSize(0,0,sw,sh);
					#ifndef WIN32
						m_ChildViewList[m_SubViewMaximized]->SetWindowSize(sw,sh);
					#endif
        }
        else
        {
          m_ChildViewList[i]->GetWindow()->SetSize(0,0,0,0);
					#ifndef WIN32
						m_ChildViewList[i]->SetWindowSize(0,0);
					#endif
        }
        i++;
      }
    }
  }
}
//----------------------------------------------------------------------------
void mafViewCompound::LayoutSubView(int width, int height)
//----------------------------------------------------------------------------
{
  // this implement the Fixed SubViews Layout
  int border = 2;
  int x_pos, y_pos, r, c, i;

  if (m_LayoutConfiguration == GRID_LAYOUT)
  {
    int step_width  = (width-border)  / m_ViewColNum;
    int step_height = (height-2*border)/ m_ViewRowNum;

    i = 0;
    for (r=0; r<m_ViewRowNum; r++)
    {
      for (c=0; c<m_ViewColNum; c++)
      {
        x_pos = c*(step_width + border);
        y_pos = r*(step_height + border);
        m_ChildViewList[i]->GetWindow()->SetSize(x_pos,y_pos,step_width,step_height);
				#ifndef WIN32
					m_ChildViewList[i]->SetWindowSize(step_width,step_height);
				#endif
        i++;
      }
    }
  }
  else if (m_LayoutConfiguration == LAYOUT_1)
  {
    int step_width  = (width-border)  / (m_NumOfChildView - 1);
    int step_height = (height-2*border)/ 3*2;
    m_ChildViewList[0]->GetWindow()->SetSize(0,0,width,step_height);
		#ifndef WIN32
			m_ChildViewList[0]->SetWindowSize(width,step_height);
		#endif
    i = 1;
    for (c = 0; c < m_NumOfChildView - 1; c++)
    {
      x_pos = c*(step_width + border);
      y_pos = step_height;
      m_ChildViewList[i]->GetWindow()->SetSize(x_pos,y_pos,step_width,height - step_height);
			#ifndef WIN32
				m_ChildViewList[i]->SetWindowSize(step_width,height - step_height);
			#endif
      i++;
    }
  }
  else if (m_LayoutConfiguration == LAYOUT_2)
  {
    int step_width  = (width-border)  / 3*2;
    int step_height = (height-2*border)/ (m_NumOfChildView - 1);
    m_ChildViewList[0]->GetWindow()->SetSize(0,0,step_width, height);
		#ifndef WIN32
			m_ChildViewList[0]->SetWindowSize(step_width,height);
		#endif
    i = 1;
    for (r = 0; r < m_NumOfChildView - 1; r++)
    {
      x_pos = step_width;
      y_pos = r*(step_height + border);
      m_ChildViewList[i]->GetWindow()->SetSize(x_pos,y_pos,width - step_width,step_height);
				#ifndef WIN32
					m_ChildViewList[i]->SetWindowSize(width - step_width,step_height);
				#endif
      i++;
    }
  }
  else
  {
    LayoutSubViewCustom(width,height);
  }
}
//----------------------------------------------------------------------------
void mafViewCompound::SetMouse(mafDeviceButtonsPadMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
  for(int i=0; i<m_NumOfChildView; i++)
  {
    ((mafViewVTK *)m_ChildViewList[i])->SetMouse(mouse);
  }
}
//----------------------------------------------------------------------------
mafView *mafViewCompound::GetSubView()
//----------------------------------------------------------------------------
{
  mafRWIBase *rwi = m_Mouse->GetRWI();
  if (rwi)
  {
    for(int i=0; i<m_NumOfChildView; i++)
    {
      if (m_ChildViewList[i]->IsMAFType(mafViewCompound))
      {
        if(((mafViewCompound *)m_ChildViewList[i])->GetSubView()->GetRWI()==rwi)
        {
          return ((mafViewCompound *)m_ChildViewList[i])->GetSubView();
        }
      }
      else if (((mafViewVTK *)m_ChildViewList[i])->GetRWI() == rwi)
      {
        return m_ChildViewList[i];
      }
    }
  }
  return m_ChildViewList[m_DefauldChildView];
}
//----------------------------------------------------------------------------
mafView *mafViewCompound::GetSubView(int idx)
//----------------------------------------------------------------------------
{
  if (idx >= 0 && idx < m_NumOfChildView)
  {
    return m_ChildViewList[idx];
  }
  return m_ChildViewList[m_DefauldChildView];
}
//----------------------------------------------------------------------------
int mafViewCompound::GetSubViewIndex()
//----------------------------------------------------------------------------
{
  mafRWIBase *rwi = m_Mouse->GetRWI();
  if (rwi)
  {
    for(int i=0; i<m_NumOfChildView; i++)
    {
      if (((mafViewVTK *)m_ChildViewList[i])->GetRWI() == rwi)
      {
        return i;
      }
    }
  }
  return m_DefauldChildView;
}
//----------------------------------------------------------------------------
void mafViewCompound::MaximizeSubView(int subview_id, bool maximize)
//----------------------------------------------------------------------------
{
  if (m_SubViewMaximized != -1 && maximize)
  {
    return;
  }
  if (subview_id < 0 || subview_id >= m_NumOfChildView)
  {
    mafMessage("Wrong sub-view id !!");
    return;
  }
  m_SubViewMaximized = maximize ? subview_id : -1;
  OnLayout();
}
//----------------------------------------------------------------------------
bool mafViewCompound::Pick(int x, int y)
//----------------------------------------------------------------------------
{
  mafView *sub_view = GetSubView();
  if (sub_view)
  {
    return sub_view->Pick(x,y);
  }
  return false;
}
//----------------------------------------------------------------------------
bool mafViewCompound::Pick(mafMatrix &m)
//----------------------------------------------------------------------------
{
  mafView *sub_view = GetSubView();
  if (sub_view)
  {
    return sub_view->Pick(m);
  }
  return false;
}
//----------------------------------------------------------------------------
void mafViewCompound::GetPickedPosition(double pos[3])
//----------------------------------------------------------------------------
{
  mafView *sub_view = GetSubView();
  if (sub_view)
  {
    sub_view->GetPickedPosition(pos);
  }
}
//----------------------------------------------------------------------------
mafVME *mafViewCompound::GetPickedVme()
//----------------------------------------------------------------------------
{
  mafView *sub_view = GetSubView();
  if (sub_view)
  {
    return sub_view->GetPickedVme();
  }
  return NULL;
}
//----------------------------------------------------------------------------
void mafViewCompound::Print(wxDC *dc, wxRect margins)
//----------------------------------------------------------------------------
{
  if (m_NumOfPluggedChildren == 0)
  {
    return;
  }

  wxBitmap image;
  GetImage(image/*, 2*/);
  PrintBitmap(dc, margins, &image);
}
//----------------------------------------------------------------------------
void mafViewCompound::GetImage(wxBitmap &bmp, int magnification)
//----------------------------------------------------------------------------
{
  wxSize vsz = this->GetWindow()->GetSize();
  wxBitmap compoundImage = wxBitmap(magnification*vsz.GetWidth(),magnification*vsz.GetHeight(),24);
  wxMemoryDC compoundDC;
  compoundDC.SelectObject(compoundImage);
  compoundDC.SetBackground(*wxWHITE_BRUSH);
  compoundDC.Clear();
  // this implement the Fixed SubViews Print

  int x_pos, y_pos;
  wxMemoryDC subViewDC;
  for (int i=0; i<m_NumOfChildView; i++)
  {
    wxSize win_size = m_ChildViewList[i]->GetWindow()->GetSize();
    if (win_size.GetWidth() == 0 || win_size.GetHeight() == 0) continue;
    wxBitmap image;
    m_ChildViewList[i]->GetImage(image, magnification);
    float iw = image.GetWidth();
    float ih = image.GetHeight();
    m_ChildViewList[i]->GetWindow()->GetPosition(&x_pos,&y_pos);
    subViewDC.SelectObject(image);
    compoundDC.Blit(magnification * x_pos,magnification * y_pos,iw,ih,&subViewDC,0,0);
  }

  compoundDC.SelectObject(wxNullBitmap);
  bmp = compoundImage;
}
//----------------------------------------------------------------------------
void mafViewCompound::LinkView(bool link_camera)
//----------------------------------------------------------------------------
{
  mafView *cv = NULL;
  for (int i=0; i<m_NumOfChildView; i++)
  {
    if (cv = mafViewVTK::SafeDownCast(m_ChildViewList[i])) 
    {
      ((mafViewVTK *)cv)->LinkView(link_camera);
    }
    else if (cv = mafViewCompound::SafeDownCast(m_ChildViewList[i])) 
    {
      ((mafViewCompound *)cv)->LinkView(link_camera);
    }
  }
}
//----------------------------------------------------------------------------
void mafViewCompound::OptionsUpdate()
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->OptionsUpdate();
}
