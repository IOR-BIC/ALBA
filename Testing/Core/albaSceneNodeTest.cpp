/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSceneNodeTest
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
#include "albaSceneNodeTest.h"
#include "albaSceneNode.h"
#include "albaVMESurface.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void albaSceneNodeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaSceneNodeTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Vme = albaVMESurface::New();
}
//----------------------------------------------------------------------------
void albaSceneNodeTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_Vme);
}
//----------------------------------------------------------------------------
void albaSceneNodeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaSceneNode n(NULL,NULL,m_Vme,NULL);
}
//----------------------------------------------------------------------------
void albaSceneNodeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSceneNode *n = new albaSceneNode(NULL, NULL, m_Vme, NULL);
  delete n;
}
//----------------------------------------------------------------------------
void albaSceneNodeTest::TestIsVisible()
//----------------------------------------------------------------------------
{
  albaSceneNode *n = new albaSceneNode(NULL, NULL, m_Vme, NULL);
  result = !n->IsVisible();
  TEST_RESULT;
  delete n;
}
