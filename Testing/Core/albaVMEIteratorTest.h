/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEIteratorTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEIteratorTest_H__
#define __CPP_UNIT_albaVMEIteratorTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaVMEIteratorTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEIteratorTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(GetFirstNodeTest);
  CPPUNIT_TEST(GetLastNodeTest);
  CPPUNIT_TEST(GetNextNodeTest);
  CPPUNIT_TEST(GetPreviousNodeTest);
  CPPUNIT_TEST(IsVisibleTest);
  CPPUNIT_TEST(SetRootNodeTest);
  CPPUNIT_TEST(GetTraversalModeTest);
  CPPUNIT_TEST(SetTraversalModeToPreOrderTest);
  CPPUNIT_TEST(SetTraversalModeToPostOrderTest);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void GetFirstNodeTest();
  void GetLastNodeTest();
  void GetNextNodeTest();
  void GetPreviousNodeTest();
  void IsVisibleTest();
  void SetRootNodeTest();
  void GetTraversalModeTest();
  void IsDoneWithTraversalTest();
  void TestBuildAndDestroyATree();
  void SetTraversalModeToPreOrderTest();
  void SetTraversalModeToPostOrderTest();

  bool result;
};

#endif
