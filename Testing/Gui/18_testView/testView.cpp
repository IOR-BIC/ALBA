/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testView
 Authors: Silvano Imboden
 
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

#include "testView.h"
#include "albaVME.h"
#include "albaPipe.h"
#include "albaPipeFactory.h"

//----------------------------------------------------------------------------
albaView *testView::Copy(albaObserver *Listener)
//----------------------------------------------------------------------------
{
  testView *v = new testView(m_Label);
  v->m_Listener = Listener;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
int testView::GetNodeStatus(albaNode *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg);
  albaSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n);

  if(!vme->IsA("albaVME"))
    return NODE_NON_VISIBLE;

  albaVME *v = ((albaVME*)vme);
  if( v->GetVisualPipe() == "" )  
    return NODE_NON_VISIBLE;
  else
    return (n->IsVisible()) ? NODE_VISIBLE_ON :  NODE_VISIBLE_OFF;
}
//----------------------------------------------------------------------------
void testView::VmeCreatePipe(albaNode *vme)
//----------------------------------------------------------------------------
{
  albaSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->m_Pipe);
  assert(vme->IsA("albaVME"));
  albaVME *v = ((albaVME*)vme);

  albaObject *obj= NULL;
  albaString pipe_name = "";
  albaString vme_type = v->GetTypeName(); // Paolo 2005-04-23 Just to try PlugVisualPipe 
                                        // (to be replaced with something that extract the type of the vme)

  albaPipeFactory *pipe_factory  = albaPipeFactory::GetInstance();
  assert(pipe_factory!=NULL);
  if (!m_PipeMap.empty() && (m_PipeMap[vme_type].m_Visibility == NODE_VISIBLE_ON))  // Paolo 2005-04-23
  {
    // keep the visual pipe from the view's visual pipe map
    obj = pipe_factory->CreateInstance(m_PipeMap[vme_type].m_PipeName);
  }
  else
  {
    // keep the default visual pipe from the vme
    pipe_name = v->GetVisualPipe();
    assert( pipe_name != "" );
    obj = pipe_factory->CreateInstance(pipe_name);
    assert(obj);
  }
  albaPipe *pipe = (albaPipe*)obj;
  
  pipe->Create(n);

  n->m_Pipe = (albaPipe*)pipe;
}
//----------------------------------------------------------------------------
void testView::VmeDeletePipe(albaNode *vme)
//----------------------------------------------------------------------------
{
  albaSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}

