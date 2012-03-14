/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafNodeGenericTest.cpp,v $
Language:  C++
Date:      $Date: 2011-05-25 11:58:32 $
Version:   $Revision: 1.1.2.2 $
Authors:   Roberto Mucci
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
#include "mafNodeGenericTest.h"

#include "mafNodeGeneric.h"
#include "mafTagArray.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);

//-------------------------------------------------------------------------
/** class for testing re-parenting. */
class mafNodeTest: public mafNode
  //-------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafNodeTest,mafNode);
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeTest);
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafNodeGenericTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafNodeGenericTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafNodeGenericTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafNodeGenericTest::TestMemoryAllocation()
//----------------------------------------------------------------------------
{
  mafNodeGeneric *node = mafNodeGeneric::New();
  node->Delete();
}
//----------------------------------------------------------------------------
void mafNodeGenericTest::TestCleanTree()
//----------------------------------------------------------------------------
{
  mafNodeGeneric *node = mafNodeGeneric::New();
  
  mafSmartPointer<mafTagArray> ta;
  ta->SetName("My Attribute");
  node->SetAttribute("My Attribute", ta);
  
  mafSmartPointer<mafNodeTest> na;

  na->ReparentTo(node);
  node->CleanTree();
  result = node->IsEmpty();
  TEST_RESULT;

  mafAttribute *a = node->GetAttribute("My Attribute");
  result = a == NULL;

  node->Delete();
}
