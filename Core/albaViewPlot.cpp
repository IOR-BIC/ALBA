/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewPlot
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include "albaViewPlot.h"

#include "albaDeviceButtonsPadMouse.h"
#include "albaGUI.h"
#include "albaPipe.h"
#include "albaPipeFactory.h"
#include "albaVME.h"

#include <wx/dc.h>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewPlot);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewPlot::albaViewPlot(const wxString &label)
:albaView(label)
{
  m_Sg = NULL;
}
//----------------------------------------------------------------------------
albaViewPlot::~albaViewPlot() 
{
  m_PipeMap.clear();
  cppDEL(m_Sg);
}

//----------------------------------------------------------------------------
void albaViewPlot::PlugVisualPipe(albaString vme_type, albaString pipe_type, long visibility)
{
  albaVisualPipeInfo plugged_pipe;
  plugged_pipe.m_PipeName = pipe_type;
  plugged_pipe.m_Visibility = visibility;
  m_PipeMap[vme_type] = plugged_pipe;
}

//----------------------------------------------------------------------------
albaView *albaViewPlot::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewPlot *v = new albaViewPlot(m_Label);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaViewPlot::Create()
{
  m_Sg  = new albaSceneGraph(this,NULL);
  m_Sg->SetListener(this);
}
//----------------------------------------------------------------------------
void albaViewPlot::SetMouse(albaDeviceButtonsPadMouse *mouse)
{
}

//----------------------------------------------------------------------------
void albaViewPlot::VmeAdd(albaVME *vme)
{
  assert(m_Sg); 
  m_Sg->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void albaViewPlot::VmeShow(albaVME *vme, bool show)	
{
	assert(m_Sg); 
	m_Sg->VmeShow(vme,show);
}

//----------------------------------------------------------------------------
void albaViewPlot::VmeUpdateProperty(albaVME *vme, bool fromTag)
{
	assert(m_Sg);
	m_Sg->VmeUpdateProperty(vme, fromTag);
}

//----------------------------------------------------------------------------
int  albaViewPlot::GetNodeStatus(albaVME *vme)
{
  int status = m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
  if (!m_PipeMap.empty())
  {
    albaString vme_type = vme->GetTypeName();
    if(m_PipeMap[vme_type].m_Visibility == NON_VISIBLE)
    {
      status = NODE_NON_VISIBLE;
    }
    else if (m_PipeMap[vme_type].m_Visibility == MUTEX)
    {
      albaSceneNode *n = m_Sg->Vme2Node(vme);
      n->SetMutex(true);
      status = m_Sg->GetNodeStatus(vme);
    }
  }
  return status;
}
//----------------------------------------------------------------------------
void albaViewPlot::VmeRemove(albaVME *vme)
{
  assert(m_Sg); 
  m_Sg->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void albaViewPlot::VmeSelect(albaVME *vme, bool select)
{
  assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);
}
//----------------------------------------------------------------------------
albaPipe* albaViewPlot::GetNodePipe(albaVME *vme)
{
   assert(m_Sg);
   albaSceneNode *n = m_Sg->Vme2Node(vme);
   if(!n) return NULL;
   return n->GetPipe();
}
//----------------------------------------------------------------------------
void albaViewPlot::GetVisualPipeName(albaVME *vme, albaString &pipe_name)
{
  albaString vme_type = vme->GetTypeName();
  if (!m_PipeMap.empty())
  {
    // pick up the visual pipe from the view's visual pipe map
    pipe_name = m_PipeMap[vme_type].m_PipeName;
  }
  if(pipe_name.IsEmpty())
  {
    // pick up the default visual pipe from the vme
    pipe_name = vme->GetVisualPipe();
  }
}
//----------------------------------------------------------------------------
void albaViewPlot::VmeCreatePipe(albaVME *vme)
{
  albaString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  if (pipe_name != "")
  {
    albaPipeFactory *pipe_factory  = albaPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    albaObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    albaPipe *pipe = (albaPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      albaSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->GetPipe());
      pipe->Create(n);
    }
    else
    {
      albaErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
    }
  }
}
//----------------------------------------------------------------------------
void albaViewPlot::VmeDeletePipe(albaVME *vme)
{
  albaSceneNode *n = m_Sg->Vme2Node(vme);
	n->DeletePipe();
}
//-------------------------------------------------------------------------
albaGUI *albaViewPlot::CreateGui()
{
  assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

  return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewPlot::OnEvent(albaEventBase *alba_event)
{
  /*if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
      default:
        albaEventMacro(*alba_event);
      break;
    }
  }
  else
  {
    albaEventMacro(*alba_event);
  }*/
  Superclass::OnEvent(alba_event);
}

//----------------------------------------------------------------------------
void albaViewPlot::SetBackgroundColor(wxColor color)
{
	m_BackgroundColor = color;
// 	m_Rwi->SetBackgroundColor(color);
// 	m_Rwi->CameraUpdate();
}
