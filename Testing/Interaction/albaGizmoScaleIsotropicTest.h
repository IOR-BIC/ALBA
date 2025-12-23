/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleIsotropicTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoScaleIsotropicTest_H__
#define __CPP_UNIT_albaGizmoScaleIsotropicTest_H__

#include "albaTest.h"

class albaGizmoScaleIsotropic;
class albaVMESurface;
class albaVMERoot;

class albaGizmoScaleIsotropicTest : public albaTest
{

public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE(albaGizmoScaleIsotropicTest);
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructor);
  CPPUNIT_TEST(TestSetGetCubeLength);
  CPPUNIT_TEST(TestSetListener);
  CPPUNIT_TEST(TestSetInput);
  CPPUNIT_TEST(TestSetGetAbsPose);
  CPPUNIT_TEST(TestShow);
  CPPUNIT_TEST_SUITE_END();

protected:

  void CreateTestData();

  void TestFixture();
  void TestConstructor();
  void TestSetGetCubeLength();
  void TestSetListener();
  void TestSetInput();
  void TestShow();
  void TestSetGetAbsPose();

  void RenderGizmo(albaGizmoScaleIsotropic *gizmoScaleIsotropic);

  albaVMESurface *m_GizmoInputSurface;
  albaVMERoot *m_Root;

};

#endif