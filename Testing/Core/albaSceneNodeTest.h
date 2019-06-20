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

#ifndef __CPP_UNIT_albaSceneNodeTest_H__
#define __CPP_UNIT_albaSceneNodeTest_H__

#include "albaTest.h"

class albaVMESurface;

/** Test for albaSceneNode; Use this suite to trace memory problems */
class albaSceneNodeTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaSceneNodeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestIsVisible);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestIsVisible();

  albaVMESurface *m_Vme;
  bool result;
};

#endif
