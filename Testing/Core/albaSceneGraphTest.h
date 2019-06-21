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

#ifndef __CPP_UNIT_albaSceneGraphTest_H__
#define __CPP_UNIT_albaSceneGraphTest_H__

#include "albaTest.h"

class albaViewVTK;
class vtkRenderer;
class albaVMERoot;

/** Test for albaSceneGraph; Use this suite to trace memory problems */
class albaSceneGraphTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaSceneGraphTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestVmeAdd);
  CPPUNIT_TEST(TestVmeRemove);
  CPPUNIT_TEST(TestVmeSelect);
  CPPUNIT_TEST(TestVme2Node);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestVmeAdd();
  void TestVmeRemove();
  void TestVmeSelect();
  void TestVme2Node();

  albaViewVTK *m_View;
  vtkRenderer *m_Ren;
  albaVMERoot *m_Vme;
  bool result;
};


#endif
