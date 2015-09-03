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

#ifndef __CPP_UNIT_mafOpNodeRootTest_H__
#define __CPP_UNIT_mafOpNodeRootTest_H__

#include "mafTest.h"

/** Test for mafOpNodeRoot; Use this suite to trace memory problems */
class mafNodeRootTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafNodeRootTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestMemoryAllocation);
  CPPUNIT_TEST(TestCleanTree);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestMemoryAllocation();
  void TestCleanTree();

  bool result;
};

#endif
