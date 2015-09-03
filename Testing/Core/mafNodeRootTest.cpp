/*=========================================================================

 Program: MAF2
 Module: mafNodeRootTest
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
#include "mafCoreTests.h"
#include "mafNodeRootTest.h"

#include "mafNodeRoot.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void mafNodeRootTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafNodeRootTest::TestMemoryAllocation()
//----------------------------------------------------------------------------
{
  mafNodeRoot *root = mafNodeRoot::New();
  root->Delete();
}
//----------------------------------------------------------------------------
void mafNodeRootTest::TestCleanTree()
//----------------------------------------------------------------------------
{
  mafNodeRoot *root = mafNodeRoot::New();
  
  mafSmartPointer<mafNodeHelper> na;

  na->ReparentTo(root);
  root->CleanTree();
  result = root->IsEmpty();
  TEST_RESULT;

  root->Delete();
}
