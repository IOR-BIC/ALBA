/*=========================================================================

 Program: MAF2
 Module: mafPipeTest
 Authors: Daniele Giunchi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "mafPipeTest.h"

#include "mafPipe.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafVME.h"
#include "vtkRenderer.h"
#include "mafView.h"
#include "mafVMERoot.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);



//----------------------------------------------------------------------------
void mafPipeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafPipe *pipe;

  pipe = new mafPipe();

  delete pipe;
}
//----------------------------------------------------------------------------
void mafPipeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafPipe pipe;
}
//----------------------------------------------------------------------------
void mafPipeTest::OnEventTest()
//----------------------------------------------------------------------------
{
  // empty method
  //TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafPipeTest::CreateTest()
//----------------------------------------------------------------------------
{
  mafView *view = NULL;
  mafSceneNode *parent = NULL;
  vtkRenderer *renFront = NULL;
  vtkRenderer *renBack = NULL;

  mafVMERoot *vme = NULL;
  mafNEW(vme);

  mafSceneGraph *sg = new mafSceneGraph(view,renFront,renBack);
  mafSceneNode *node = new mafSceneNode(sg,parent,vme,renFront,renBack);

  mafPipe *pipe;

  pipe = new mafPipe();
  pipe->Create(node);

  result = node->m_AssemblyBack == pipe->m_AssemblyBack;
  TEST_RESULT;
  result = node->m_AssemblyFront == pipe->m_AssemblyFront;
  TEST_RESULT;
  result = node->m_Sg == pipe->m_Sg;
  TEST_RESULT;
  result = node->m_RenBack== pipe->m_RenBack;
  TEST_RESULT;
  result = node->m_RenFront== pipe->m_RenFront;
  TEST_RESULT;
  result = node->m_Vme == pipe->m_Vme;
  TEST_RESULT;
  result = pipe->m_Selected == false;
  TEST_RESULT;

  mafDEL(vme);
  delete pipe;
  delete node;
  delete sg;

}
//----------------------------------------------------------------------------
void mafPipeTest::GetSetListenerTest()
//----------------------------------------------------------------------------
{
  mafPipe pipe;
  mafPipe *pipeListener = new mafPipe();
  pipe.SetListener(pipeListener);
  mafObserver *observer = pipe.GetListener();
  result = observer == pipeListener;
  delete pipeListener;
  TEST_RESULT;
}

//----------------------------------------------------------------------------
void mafPipeTest::SelectTest()
//----------------------------------------------------------------------------
{
  //empty method
  //TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafPipeTest::UpdateProperty()
//----------------------------------------------------------------------------
{
  //empty method
  //TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafPipeTest::GetGui()
//----------------------------------------------------------------------------
{
  //not tested for incompatibility with wx
  //TEST_RESULT;
}
//----------------------------------------------------------------------------
void mafPipeTest::DeleteGui()
//----------------------------------------------------------------------------
{
  //not tested for incompatibility with wx
  //TEST_RESULT;
}
