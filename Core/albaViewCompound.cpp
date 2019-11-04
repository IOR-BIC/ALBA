/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewCompound
 Authors: Paolo Quadrani
 
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

#include "albaViewCompound.h"
#include "albaViewVTK.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaGUIViewWin.h"
#include "albaGUI.h"
#include "albaInteractor.h"
#include "albaAvatar.h"
#include "albaAvatar3D.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaDeviceButtonsPadMouse.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewCompound::albaViewCompound(const wxString &label, int num_row, int num_col)
: albaView(label)
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
albaViewCompound::~albaViewCompound()
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
albaView *albaViewCompound::Copy(albaObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewCompound *v = new albaViewCompound(m_Label, m_ViewRowNum, m_ViewColNum);
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
void albaViewCompound::PlugChildView(albaView *child)
//----------------------------------------------------------------------------
{
  if (child)
  {
    m_PluggedChildViewList.push_back(child);
    m_NumOfPluggedChildren++;
  }
}
//----------------------------------------------------------------------------
void albaViewCompound::Create()
//----------------------------------------------------------------------------
{
  albaGUIViewWin *w = new albaGUIViewWin(albaGetFrame(),-1);
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
void albaViewCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
  /*m_GuiView = new albaGUI(this);
  m_GuiView->Label("Compound View's GUI",true);
  m_GuiView->Reparent(m_Win);*/
}
//----------------------------------------------------------------------------
void albaViewCompound::VmeAdd(albaVME *vme)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void albaViewCompound::VmeRemove(albaVME *vme)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void albaViewCompound::VmeSelect(albaVME *vme, bool select)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeSelect(vme, select);
}
//----------------------------------------------------------------------------
void albaViewCompound::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
	for (int i = 0; i < m_NumOfChildView; i++)
		m_ChildViewList[i]->VmeShow(vme, show);
}

//----------------------------------------------------------------------------
void albaViewCompound::VmeUpdateProperty(albaVME *vme, bool fromTag /*= false*/)
{
	for (int i = 0; i < m_NumOfChildView; i++)
		m_ChildViewList[i]->VmeUpdateProperty(vme, fromTag);
}

//----------------------------------------------------------------------------
int albaViewCompound::GetNodeStatus(albaVME *vme)
//----------------------------------------------------------------------------
{
  // should be redefined for compounded views
  return m_ChildViewList[m_DefauldChildView]->GetNodeStatus(vme);
}

//----------------------------------------------------------------------------
bool albaViewCompound::IsVmeShowed(albaVME *vme)
{
	return m_ChildViewList[m_DefauldChildView]->GetNodePipe(vme) != NULL;
}

//----------------------------------------------------------------------------
void albaViewCompound::CameraReset(albaVME *vme)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->CameraReset(vme);
}
//----------------------------------------------------------------------------
void albaViewCompound::CameraUpdate()
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->CameraUpdate();
}
//----------------------------------------------------------------------------
albaSceneGraph *albaViewCompound::GetSceneGraph()
//----------------------------------------------------------------------------
{
  //return ((albaViewVTK *)m_ChildViewList[m_DefauldChildView])->GetSceneGraph();
  return GetSubView()->GetSceneGraph();
}
//----------------------------------------------------------------------------
albaRWIBase *albaViewCompound::GetRWI()
//----------------------------------------------------------------------------
{
  //return ((albaViewVTK *)m_ChildViewList[m_DefauldChildView])->GetRWI();
  return GetSubView()->GetRWI();
}
//----------------------------------------------------------------------------
bool albaViewCompound::FindPokedVme(albaDevice *device,albaMatrix &point_pose,vtkProp3D *&picked_prop,albaVME *&picked_vme,albaInteractor *&picked_behavior)
//----------------------------------------------------------------------------
{
  albaViewVTK *v = albaViewVTK::SafeDownCast(GetSubView());
  if (v)
    return v->FindPokedVme(device,point_pose,picked_prop,picked_vme,picked_behavior);
  else
    return false;
}
//----------------------------------------------------------------------------
void albaViewCompound::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId()) 
  {
    case ID_DEFAULT_CHILD_VIEW:
    {
      albaSceneGraph *sg = GetSceneGraph();
      if(sg)
      {
        albaVME *vme = sg->GetSelectedVme();
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
    default:
      albaView::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
albaGUI* albaViewCompound::CreateGui()
//-------------------------------------------------------------------------
{
  albaString childview_tooltip;
  childview_tooltip = "set the default child view";

  wxString layout_choices[4] = {"default","layout 1","layout 2", "custom"};

  assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

  m_Gui->Integer(ID_DEFAULT_CHILD_VIEW,"default child", &m_DefauldChildView, 0, m_NumOfChildView, childview_tooltip);
  m_Gui->Combo(ID_LAYOUT_CHOOSER,"layout",&m_LayoutConfiguration,4,layout_choices);
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewCompound::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  m_Size = event.GetSize();
  OnLayout();
}
//----------------------------------------------------------------------------
void albaViewCompound::OnLayout()
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
void albaViewCompound::LayoutSubView(int width, int height)
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
}
//----------------------------------------------------------------------------
void albaViewCompound::SetMouse(albaDeviceButtonsPadMouse *mouse)
//----------------------------------------------------------------------------
{
  m_Mouse = mouse;
  for(int i=0; i<m_NumOfChildView; i++)
  {
    ((albaViewVTK *)m_ChildViewList[i])->SetMouse(mouse);
  }
}
//----------------------------------------------------------------------------
albaView *albaViewCompound::GetSubView()
//----------------------------------------------------------------------------
{
  albaRWIBase *rwi = m_Mouse->GetRWI();
  if (rwi)
  {
    for(int i=0; i<m_NumOfChildView; i++)
    {
      if (m_ChildViewList[i]->IsALBAType(albaViewCompound))
      {
        if(((albaViewCompound *)m_ChildViewList[i])->GetSubView()->GetRWI()==rwi)
        {
          return ((albaViewCompound *)m_ChildViewList[i])->GetSubView();
        }
      }
      else if (((albaViewVTK *)m_ChildViewList[i])->GetRWI() == rwi)
      {
        return m_ChildViewList[i];
      }
    }
  }
  return m_ChildViewList[m_DefauldChildView];
}
//----------------------------------------------------------------------------
albaView *albaViewCompound::GetSubView(int idx)
//----------------------------------------------------------------------------
{
  if (idx >= 0 && idx < m_NumOfChildView)
  {
    return m_ChildViewList[idx];
  }
  return m_ChildViewList[m_DefauldChildView];
}
//----------------------------------------------------------------------------
int albaViewCompound::GetSubViewIndex()
//----------------------------------------------------------------------------
{
  albaRWIBase *rwi = m_Mouse->GetRWI();
  if (rwi)
  {
    for(int i=0; i<m_NumOfChildView; i++)
    {
      if (((albaViewVTK *)m_ChildViewList[i])->GetRWI() == rwi)
      {
        return i;
      }
    }
  }
  return m_DefauldChildView;
}
//----------------------------------------------------------------------------
void albaViewCompound::MaximizeSubView(int subview_id, bool maximize)
//----------------------------------------------------------------------------
{
  if (m_SubViewMaximized != -1 && maximize)
  {
    return;
  }
  if (subview_id < 0 || subview_id >= m_NumOfChildView)
  {
    albaMessage("Wrong sub-view id !!");
    return;
  }
  m_SubViewMaximized = maximize ? subview_id : -1;
  OnLayout();
}
//----------------------------------------------------------------------------
bool albaViewCompound::Pick(int x, int y)
//----------------------------------------------------------------------------
{
  albaView *sub_view = GetSubView();
  if (sub_view)
  {
    return sub_view->Pick(x,y);
  }
  return false;
}
//----------------------------------------------------------------------------
bool albaViewCompound::Pick(albaMatrix &m)
//----------------------------------------------------------------------------
{
  albaView *sub_view = GetSubView();
  if (sub_view)
  {
    return sub_view->Pick(m);
  }
  return false;
}
//----------------------------------------------------------------------------
void albaViewCompound::GetPickedPosition(double pos[3])
//----------------------------------------------------------------------------
{
  albaView *sub_view = GetSubView();
  if (sub_view)
  {
    sub_view->GetPickedPosition(pos);
  }
}
//----------------------------------------------------------------------------
albaVME *albaViewCompound::GetPickedVme()
//----------------------------------------------------------------------------
{
  albaView *sub_view = GetSubView();
  if (sub_view)
  {
    return sub_view->GetPickedVme();
  }
  return NULL;
}
//----------------------------------------------------------------------------
void albaViewCompound::GetImage(wxBitmap &bmp, int magnification)
//----------------------------------------------------------------------------
{
  wxSize vsz = this->GetWindow()->GetSize();
  bmp = wxBitmap(magnification*vsz.GetWidth(),magnification*vsz.GetHeight(),24);
  wxMemoryDC compoundDC;
  compoundDC.SelectObject(bmp);
  compoundDC.SetBackground(*wxGREY_BRUSH);
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
}

//----------------------------------------------------------------------------
void albaViewCompound::OptionsUpdate()
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->OptionsUpdate();
}

//----------------------------------------------------------------------------
void albaViewCompound::SetBackgroundColor(wxColor color)
{
	m_BackgroundColor = color;

	for (int i = 0; i < m_NumOfChildView; i++)
	{
		m_ChildViewList[i]->SetBackgroundColor(color);
	}
}
//----------------------------------------------------------------------------
void albaViewCompound::SetSubViewBackgroundColor(int subView, wxColor color)
{
	if (subView >= 0 && subView < m_NumOfChildView)
		m_ChildViewList[subView]->SetBackgroundColor(color);
}
//----------------------------------------------------------------------------
wxColor albaViewCompound::GetSubViewBackgroundColor(int subView)
{
	wxColor color = wxColor();

	if (subView >= 0 && subView < m_NumOfChildView)
		color = m_ChildViewList[subView]->GetBackgroundColor();

	return color;
}

//----------------------------------------------------------------------------
void albaViewCompound::SetLayoutConfiguration(int layoutConfiguration)
{
	m_LayoutConfiguration = layoutConfiguration;
	if(m_Gui)
		m_Gui->Update();
	OnLayout();
	CameraUpdate();
}
