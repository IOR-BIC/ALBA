/*=========================================================================

 Program: MAF2
 Module: testView
 Authors: Silvano Imboden
 
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

#include "testView.h"
#include "mafVME.h"
#include "mafPipe.h"
#include "mafPipeFactory.h"

//----------------------------------------------------------------------------
mafView *testView::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  testView *v = new testView(m_Label);
  v->m_Listener = Listener;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
int testView::GetNodeStatus(mafVME *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg);
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n);

  if( vme->GetVisualPipe() == "" )  
    return NODE_NON_VISIBLE;
  else
    return (n->IsVisible()) ? NODE_VISIBLE_ON :  NODE_VISIBLE_OFF;
}
//----------------------------------------------------------------------------
void testView::VmeCreatePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->GetPipe());
  assert(vme->IsA("mafVME"));
  
  mafObject *obj= NULL;
  mafString pipe_name = "";
  mafString vme_type = vme->GetTypeName(); // Paolo 2005-04-23 Just to try PlugVisualPipe 
                                        // (to be replaced with something that extract the type of the vme)

  mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
  assert(pipe_factory!=NULL);
  if (!m_PipeMap.empty() && (m_PipeMap[vme_type].m_Visibility == NODE_VISIBLE_ON))  // Paolo 2005-04-23
  {
    // keep the visual pipe from the view's visual pipe map
    obj = pipe_factory->CreateInstance(m_PipeMap[vme_type].m_PipeName);
  }
  else
  {
    // keep the default visual pipe from the vme
    pipe_name = vme->GetVisualPipe();
    assert( pipe_name != "" );
    obj = pipe_factory->CreateInstance(pipe_name);
    assert(obj);
  }
  mafPipe *pipe = (mafPipe*)obj;
  
  pipe->Create(n);
}
//----------------------------------------------------------------------------
void testView::VmeDeletePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && n->GetPipe());
	n->DeletePipe();
}

