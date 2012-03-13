/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafSceneNodeTest.cpp,v $
Language:  C++
Date:      $Date: 2008-06-24 17:19:32 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2008
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafSceneNodeTest.h"
#include "mafSceneNode.h"
#include "mafVMESurface.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//----------------------------------------------------------------------------
void mafSceneNodeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafSceneNodeTest::setUp()
//----------------------------------------------------------------------------
{
  m_Vme = mafVMESurface::New();
  result = false;
}
//----------------------------------------------------------------------------
void mafSceneNodeTest::tearDown()
//----------------------------------------------------------------------------
{
  mafDEL(m_Vme);
}
//----------------------------------------------------------------------------
void mafSceneNodeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafSceneNode n(NULL,NULL,m_Vme,NULL);
}
//----------------------------------------------------------------------------
void mafSceneNodeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSceneNode *n = new mafSceneNode(NULL, NULL, m_Vme, NULL);
  delete n;
}
//----------------------------------------------------------------------------
void mafSceneNodeTest::TestIsVisible()
//----------------------------------------------------------------------------
{
  mafSceneNode *n = new mafSceneNode(NULL, NULL, m_Vme, NULL);
  result = !n->IsVisible();
  TEST_RESULT;
  delete n;
}
