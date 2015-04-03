/*=========================================================================

 Program: MAF2
 Module: mafNodeGenericTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
#include "mafCoreTests.h"
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
  
  mafSmartPointer<mafNodeHelper> na;

  na->ReparentTo(node);
  node->CleanTree();
  result = node->IsEmpty();
  TEST_RESULT;

  mafAttribute *a = node->GetAttribute("My Attribute");
  result = a == NULL;

  node->Delete();
}
