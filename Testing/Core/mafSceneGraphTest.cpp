/*=========================================================================

 Program: MAF2
 Module: mafSceneGraphTest
 Authors: Paolo Quadrani
 
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
#include "mafSceneGraphTest.h"
#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafViewVTK.h"

#include "mafVMESurface.h"
#include "mafVMERoot.h"
#include "vtkRenderer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafSceneGraphTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafSceneGraphTest::setUp()
//----------------------------------------------------------------------------
{
  m_View = new mafViewVTK();
  m_Ren = vtkRenderer::New();
  m_Vme = mafVMERoot::New();

  result = false;
}
//----------------------------------------------------------------------------
void mafSceneGraphTest::tearDown()
//----------------------------------------------------------------------------
{
  cppDEL(m_View);
  vtkDEL(m_Ren);
  mafDEL(m_Vme);
}
//----------------------------------------------------------------------------
void mafSceneGraphTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafSceneGraph sg(NULL,NULL);
}
//----------------------------------------------------------------------------
void mafSceneGraphTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSceneGraph *sg = new mafSceneGraph(NULL,NULL);
  delete sg;
}
//----------------------------------------------------------------------------
void mafSceneGraphTest::TestVmeAdd()
//----------------------------------------------------------------------------
{
  mafSceneGraph *sg = new mafSceneGraph(m_View, m_Ren);
  sg->VmeAdd(m_Vme);
  mafSceneNode *nl = sg->GetNodeList();
  
  for(mafSceneNode *n = nl; n; n=n->m_Next)
    if(n->m_Vme == m_Vme)
      result = true;
  TEST_RESULT;

  delete sg;
}

//----------------------------------------------------------------------------
void mafSceneGraphTest::TestVmeRemove()
//----------------------------------------------------------------------------
{
  mafSceneGraph *sg = new mafSceneGraph(m_View, m_Ren);
  sg->VmeAdd(m_Vme);

  // Remove the previous added node to make the node list empty.
  sg->VmeRemove(m_Vme);
  mafSceneNode *nl = sg->GetNodeList();
  result = nl == NULL; // empty node list
  TEST_RESULT;

  delete sg;
}

//----------------------------------------------------------------------------
void mafSceneGraphTest::TestVmeSelect()
//----------------------------------------------------------------------------
{
  mafSceneGraph *sg = new mafSceneGraph(m_View, m_Ren);
  sg->VmeAdd(m_Vme);
  sg->VmeSelect(m_Vme, true);

  result = sg->GetSelectedVme()->Equals(m_Vme);
  TEST_RESULT;

  delete sg;
}

//----------------------------------------------------------------------------
void mafSceneGraphTest::TestVme2Node()
//----------------------------------------------------------------------------
{
  mafSceneGraph *sg = new mafSceneGraph(m_View, m_Ren);
  // VME not added to the SceneGraph -> no corresponding SceneNode can be found.
  result = sg->Vme2Node(m_Vme) == NULL;
  TEST_RESULT;

  sg->VmeAdd(m_Vme);
  mafSceneNode *n = sg->Vme2Node(m_Vme);
  result = n->m_Vme->Equals(m_Vme);
  TEST_RESULT;

  delete sg;
}
