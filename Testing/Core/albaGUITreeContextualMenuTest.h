/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITreeContextualMenuTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUITreeContextualMenuTest_H__
#define __CPP_UNIT_albaGUITreeContextualMenuTest_H__

#include "albaTest.h"

/** Test for albaGUITreeContextualMenu; Use this suite to trace memory problems */
class albaGUITreeContextualMenuTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaGUITreeContextualMenuTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestCreateContextualMenu);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestCreateContextualMenu();

  bool result;
};


#endif
