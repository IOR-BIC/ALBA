/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewCompound.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-04 14:48:12 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
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

#include "mafViewCompound.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mmgViewWin.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewCompound::mafViewCompound( wxString label, int num_row, int num_col, bool external)
: mafView(label, external)
//----------------------------------------------------------------------------
{
  m_ViewRowNum = num_row;
  m_ViewColNum = num_col;
  m_NumOfPluggedChildren = 0;
}
//----------------------------------------------------------------------------
mafViewCompound::~mafViewCompound()
//----------------------------------------------------------------------------
{
  m_PluggedChildViewList.clear();

  for(int i=0; i<m_ChildViewList.size(); i++)
  {
    cppDEL(m_ChildViewList[i]);
  }
  m_ChildViewList.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewCompound::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewCompound *v = new mafViewCompound(m_Label, m_ViewRowNum, m_ViewColNum, m_ExternalFlag);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PluggedChildViewList = m_PluggedChildViewList;
  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
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
  mmgViewWin *w = new mmgViewWin(mafGetFrame(),-1);
  w->SetBackgroundColour(wxColour(102,102,102));
  w->m_Owner = this;
  w->Show(false);
  m_Win = w;

  for(int i=0; i<m_NumOfPluggedChildren; i++)
  {
    m_ChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
    m_ChildViewList[i]->GetWindow()->Reparent(m_Win);
    m_ChildViewList[i]->GetWindow()->Show(true);
  }
  for (int f=m_NumOfPluggedChildren; f < m_ViewColNum * m_ViewRowNum; f++)
  {
    m_ChildViewList.push_back(m_PluggedChildViewList[m_NumOfPluggedChildren-1]->Copy(this));
    m_ChildViewList[f]->GetWindow()->Reparent(m_Win);
    m_ChildViewList[f]->GetWindow()->Show(true);
  }
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeAdd(mafNode *node)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    m_ChildViewList[i]->VmeAdd(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    m_ChildViewList[i]->VmeRemove(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    m_ChildViewList[i]->VmeSelect(node, select);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    m_ChildViewList[i]->VmeShow(node, show);
}
//----------------------------------------------------------------------------
int mafViewCompound::GetNodeStatus(mafNode *node)
//----------------------------------------------------------------------------
{
  // should be redefined for compounded views
  return m_ChildViewList[0]->GetNodeStatus(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeCreatePipe(mafNode *node)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    m_ChildViewList[i]->VmeCreatePipe(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeDeletePipe(mafNode *node)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    m_ChildViewList[i]->VmeDeletePipe(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::CameraReset(mafNode *node)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    m_ChildViewList[i]->CameraReset(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::CameraUpdate()
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    m_ChildViewList[i]->CameraUpdate();
}
//----------------------------------------------------------------------------
mafSceneGraph *mafViewCompound::GetSceneGraph()
//----------------------------------------------------------------------------
{
  return ((mafViewVTK *)m_ChildViewList[0])->GetSceneGraph();
}
//----------------------------------------------------------------------------
mafRWIBase *mafViewCompound::GetRWI()
//----------------------------------------------------------------------------
{
  return ((mafViewVTK *)m_ChildViewList[0])->GetRWI();
}
//----------------------------------------------------------------------------
void mafViewCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*maf_event);
}
//-------------------------------------------------------------------------
mmgGui* mafViewCompound::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Label("compound view default gui");
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

  // this implement the Fixed SubViews Layout
  int border = 2;
  int step_width  = (sw-border)  / m_ViewColNum;
  int step_height = (sh-2*border)/ m_ViewRowNum;
  int x_pos, y_pos;

  int i = 0;
  for (int r=0; r<m_ViewRowNum; r++)
  {
    for (int c=0; c<m_ViewColNum; c++)
    {
      x_pos = c*(step_width + border);
      y_pos = r*(step_height + border);
      m_ChildViewList[i]->GetWindow()->SetSize(x_pos,y_pos,step_width,step_height);
      i++;
    }
  }
}
//----------------------------------------------------------------------------
void mafViewCompound::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_ChildViewList.size(); i++)
    ((mafViewVTK *)m_ChildViewList[i])->SetMouse(mouse);
}
