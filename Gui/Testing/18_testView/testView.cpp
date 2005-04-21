/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testView.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 16:33:44 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
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

#include "testView.h"
#include "mafVME.h"
#include "mafPipe.h"
//----------------------------------------------------------------------------
mafView *testView::Copy(mafEventListener *Listener)
//----------------------------------------------------------------------------
{
  testView *v = new testView(m_label);
  v->m_Listener = Listener;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
int testView::GetNodeStatus(mafNode *vme)
//----------------------------------------------------------------------------
{
  assert(m_sg);
  mafSceneNode *n = m_sg->Vme2Node(vme);
  assert(n);

  if(!vme->IsA("mafVME"))
    return NODE_NON_VISIBLE;

  mafVME *v = ((mafVME*)vme);
  if(!v->HasVisualPipe())  
    return NODE_NON_VISIBLE;
  else
    return (n->IsVisible()) ? NODE_VISIBLE_ON :  NODE_VISIBLE_OFF;
}
//----------------------------------------------------------------------------
void testView::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_sg->Vme2Node(vme);
  assert(n && !n->m_pipe);
  assert(vme->IsA("mafVME"));
  mafVME *v = ((mafVME*)vme);

  n->m_pipe = v->GetVisualPipe(n);
}
//----------------------------------------------------------------------------
void testView::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_sg->Vme2Node(vme);
  assert(n && n->m_pipe);
  cppDEL(n->m_pipe);
}

