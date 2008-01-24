/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewPlot.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-24 16:05:31 $
  Version:   $Revision: 1.5 $
  Authors:   Silvano Imboden - Paolo Quadrani
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

#include "mafViewPlot.h"
#include <wx/dc.h>
#include "mafPipe.h"
#include "mafPipeFactory.h"

#include "mmdMouse.h"

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
mafView *mafViewPlot::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
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
void mafViewPlot::SetMouse(mmdMouse *mouse)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafViewPlot::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeShow(mafNode *vme, bool show)												{assert(m_Sg); m_Sg->VmeShow(vme,show);}
void mafViewPlot::VmeUpdateProperty(mafNode *vme, bool fromTag)	        {assert(m_Sg); m_Sg->VmeUpdateProperty(vme,fromTag);}
//----------------------------------------------------------------------------
int  mafViewPlot::GetNodeStatus(mafNode *vme)
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
      n->m_Mutex = true;
      status = m_Sg->GetNodeStatus(vme);
    }
  }
  return status;
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeSelect(mafNode *vme, bool select)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);
}
//----------------------------------------------------------------------------
mafPipe* mafViewPlot::GetNodePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
   assert(m_Sg);
   mafSceneNode *n = m_Sg->Vme2Node(vme);
   if(!n) return NULL;
   return n->m_Pipe;
}
//----------------------------------------------------------------------------
void mafViewPlot::GetVisualPipeName(mafNode *node, mafString &pipe_name)
//----------------------------------------------------------------------------
{
  assert(node->IsA("mafVME"));
  mafVME *v = ((mafVME*)node);

  mafString vme_type = v->GetTypeName();
  if (!m_PipeMap.empty())
  {
    // pick up the visual pipe from the view's visual pipe map
    pipe_name = m_PipeMap[vme_type].m_PipeName;
  }
  if(pipe_name.IsEmpty())
  {
    // pick up the default visual pipe from the vme
    pipe_name = v->GetVisualPipe();
  }
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeCreatePipe(mafNode *vme)
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
      assert(n && !n->m_Pipe);
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
    }
    else
    {
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
    }
  }
}
//----------------------------------------------------------------------------
void mafViewPlot::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//-------------------------------------------------------------------------
mmgGui *mafViewPlot::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
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
