/*=========================================================================

 Program: MAF2
 Module: mafViewPlot
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include "mafViewPlot.h"
#include <wx/dc.h>
#include "mafGUI.h"
#include "mafPipe.h"
#include "mafPipeFactory.h"

#include "mafDeviceButtonsPadMouse.h"

#include "mafVME.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewPlot);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewPlot::mafViewPlot(const wxString &label)
:mafView(label)
//----------------------------------------------------------------------------
{
  m_Sg = NULL;
}
//----------------------------------------------------------------------------
mafViewPlot::~mafViewPlot() 
//----------------------------------------------------------------------------
{
  m_PipeMap.clear();
  cppDEL(m_Sg);
}
//----------------------------------------------------------------------------
void mafViewPlot::PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
  mafVisualPipeInfo plugged_pipe;
  plugged_pipe.m_PipeName = pipe_type;
  plugged_pipe.m_Visibility = visibility;
  m_PipeMap[vme_type] = plugged_pipe;
}
//----------------------------------------------------------------------------
mafView *mafViewPlot::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewPlot *v = new mafViewPlot(m_Label);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewPlot::Create()
//----------------------------------------------------------------------------
{
  m_Sg  = new mafSceneGraph(this,NULL);
  m_Sg->SetListener(this);
}
//----------------------------------------------------------------------------
void mafViewPlot::SetMouse(mafDeviceButtonsPadMouse *mouse)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafViewPlot::VmeAdd(mafVME *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeShow(mafVME *vme, bool show)												{assert(m_Sg); m_Sg->VmeShow(vme,show);}
//----------------------------------------------------------------------------
void mafViewPlot::VmeUpdateProperty(mafVME *vme, bool fromTag)	        {assert(m_Sg); m_Sg->VmeUpdateProperty(vme,fromTag);}
//----------------------------------------------------------------------------
int  mafViewPlot::GetNodeStatus(mafVME *vme)
//----------------------------------------------------------------------------
{
  int status = m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
  if (!m_PipeMap.empty())
  {
    mafString vme_type = vme->GetTypeName();
    if(m_PipeMap[vme_type].m_Visibility == NON_VISIBLE)
    {
      status = NODE_NON_VISIBLE;
    }
    else if (m_PipeMap[vme_type].m_Visibility == MUTEX)
    {
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      n->SetMutex(true);
      status = m_Sg->GetNodeStatus(vme);
    }
  }
  return status;
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeRemove(mafVME *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeSelect(mafVME *vme, bool select)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);
}
//----------------------------------------------------------------------------
mafPipe* mafViewPlot::GetNodePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
   assert(m_Sg);
   mafSceneNode *n = m_Sg->Vme2Node(vme);
   if(!n) return NULL;
   return n->GetPipe();
}
//----------------------------------------------------------------------------
void mafViewPlot::GetVisualPipeName(mafVME *vme, mafString &pipe_name)
//----------------------------------------------------------------------------
{
  mafString vme_type = vme->GetTypeName();
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
void mafViewPlot::VmeCreatePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  if (pipe_name != "")
  {
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->GetPipe());
      pipe->Create(n);
      n->SetPipe(pipe);
    }
    else
    {
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
    }
  }
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeDeletePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
	n->DeletePipe();
}
//-------------------------------------------------------------------------
mafGUI *mafViewPlot::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewPlot::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  /*if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      default:
        mafEventMacro(*maf_event);
      break;
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }*/
  mafEventMacro(*maf_event);
}
//----------------------------------------------------------------------------
void mafViewPlot::Print(wxDC *dc, wxRect margins)
//----------------------------------------------------------------------------
{
  wxBitmap image;
  GetImage(image/*, 2*/);
  PrintBitmap(dc, margins, &image);
}
//----------------------------------------------------------------------------
void mafViewPlot::GetImage(wxBitmap &bmp, int magnification)
//----------------------------------------------------------------------------
{
//  bmp = *m_Rwi->m_RwiBase->GetImage(magnification);
}
