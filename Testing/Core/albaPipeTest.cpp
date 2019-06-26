/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeTest
 Authors: Daniele Giunchi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaPipeTest.h"

#include "albaPipe.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h"
#include "albaVME.h"
#include "vtkRenderer.h"
#include "albaView.h"
#include "albaVMERoot.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);



//----------------------------------------------------------------------------
void albaPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaPipe *pipe;

  pipe = new albaPipe();

  delete pipe;
}
//----------------------------------------------------------------------------
void albaPipeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaPipe pipe;
}
//----------------------------------------------------------------------------
void albaPipeTest::OnEventTest()
//----------------------------------------------------------------------------
{
  // empty method
  //TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaPipeTest::CreateTest()
//----------------------------------------------------------------------------
{
  albaView *view = NULL;
  albaSceneNode *parent = NULL;
  vtkRenderer *renFront = NULL;
  vtkRenderer *renBack = NULL;

  albaVMERoot *vme = NULL;
  albaNEW(vme);

  albaSceneGraph *sg = new albaSceneGraph(view,renFront,renBack);
  albaSceneNode *node = new albaSceneNode(sg,parent,vme,renFront,renBack);

  albaPipe *pipe;

  pipe = new albaPipe();
  pipe->Create(node);

  result = node->GetAssemblyBack() == pipe->m_AssemblyBack;
  TEST_RESULT;
  result = node->GetAssemblyFront() == pipe->m_AssemblyFront;
  TEST_RESULT;
  result = node->GetSceneGraph() == pipe->m_Sg;
  TEST_RESULT;
  result = node->GetRenBack()== pipe->m_RenBack;
  TEST_RESULT;
  result = node->GetRenFront()== pipe->m_RenFront;
  TEST_RESULT;
  result = node->GetVme() == pipe->m_Vme;
  TEST_RESULT;
  result = pipe->m_Selected == false;
  TEST_RESULT;

  albaDEL(vme);
  delete node;
  delete sg;
}
//----------------------------------------------------------------------------
void albaPipeTest::GetSetListenerTest()
//----------------------------------------------------------------------------
{
  albaPipe pipe;
  albaPipe *pipeListener = new albaPipe();
  pipe.SetListener(pipeListener);
  albaObserver *observer = pipe.GetListener();
  result = observer == pipeListener;
  delete pipeListener;
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void albaPipeTest::SelectTest()
//----------------------------------------------------------------------------
{
  //empty method
  //TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaPipeTest::UpdateProperty()
//----------------------------------------------------------------------------
{
  //empty method
  //TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaPipeTest::GetGui()
//----------------------------------------------------------------------------
{
  //not tested for incompatibility with wx
  //TEST_RESULT;
}
//----------------------------------------------------------------------------
void albaPipeTest::DeleteGui()
//----------------------------------------------------------------------------
{
  //not tested for incompatibility with wx
  //TEST_RESULT;
}
