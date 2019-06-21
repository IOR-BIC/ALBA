/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSceneGraphTest
 Authors: Paolo Quadrani
 
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
#include "albaSceneGraphTest.h"
#include "albaSceneNode.h"
#include "albaSceneGraph.h"
#include "albaViewVTK.h"

#include "albaVMESurface.h"
#include "albaVMERoot.h"
#include "vtkRenderer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaSceneGraphTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaSceneGraphTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_View = new albaViewVTK();
  m_Ren = vtkRenderer::New();
  m_Vme = albaVMERoot::New();
}
//----------------------------------------------------------------------------
void albaSceneGraphTest::AfterTest()
//----------------------------------------------------------------------------
{
  cppDEL(m_View);
  vtkDEL(m_Ren);
  albaDEL(m_Vme);
}
//----------------------------------------------------------------------------
void albaSceneGraphTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaSceneGraph sg(NULL,NULL);
}
//----------------------------------------------------------------------------
void albaSceneGraphTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSceneGraph *sg = new albaSceneGraph(NULL,NULL);
  delete sg;
}
//----------------------------------------------------------------------------
void albaSceneGraphTest::TestVmeAdd()
//----------------------------------------------------------------------------
{
  albaSceneGraph *sg = new albaSceneGraph(m_View, m_Ren);
  sg->VmeAdd(m_Vme);
  albaSceneNode *nl = sg->GetNodeList();
  
  for(albaSceneNode *n = nl; n; n=n->GetNext())
    if(n->GetVme() == m_Vme)
      result = true;
  TEST_RESULT;

  delete sg;
}

//----------------------------------------------------------------------------
void albaSceneGraphTest::TestVmeRemove()
//----------------------------------------------------------------------------
{
  albaSceneGraph *sg = new albaSceneGraph(m_View, m_Ren);
  sg->VmeAdd(m_Vme);

  // Remove the previous added node to make the node list empty.
  sg->VmeRemove(m_Vme);
  albaSceneNode *nl = sg->GetNodeList();
  result = nl == NULL; // empty node list
  TEST_RESULT;

  delete sg;
}

//----------------------------------------------------------------------------
void albaSceneGraphTest::TestVmeSelect()
//----------------------------------------------------------------------------
{
  albaSceneGraph *sg = new albaSceneGraph(m_View, m_Ren);
  sg->VmeAdd(m_Vme);
  sg->VmeSelect(m_Vme, true);

  result = sg->GetSelectedVme()->Equals(m_Vme);
  TEST_RESULT;

  delete sg;
}

//----------------------------------------------------------------------------
void albaSceneGraphTest::TestVme2Node()
//----------------------------------------------------------------------------
{
  albaSceneGraph *sg = new albaSceneGraph(m_View, m_Ren);
  // VME not added to the SceneGraph -> no corresponding SceneNode can be found.
  result = sg->Vme2Node(m_Vme) == NULL;
  TEST_RESULT;

  sg->VmeAdd(m_Vme);
  albaSceneNode *n = sg->Vme2Node(m_Vme);
  result = n->GetVme()->Equals(m_Vme);
  TEST_RESULT;

  delete sg;
}
