/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoHandleTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoHandleTest_H__
#define __CPP_UNIT_albaGizmoHandleTest_H__

#include "albaTest.h"

#include "albaVMESurface.h"
#include "albaVMERoot.h"

class albaGizmoHandleTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoHandleTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructor);
  CPPUNIT_TEST(TestSetGetInput);
  CPPUNIT_TEST(TestSetGetListener);
  CPPUNIT_TEST(TestHighlight);
  CPPUNIT_TEST(TestShow);
  CPPUNIT_TEST(TestSetGetLength);
  CPPUNIT_TEST(TestSetGetAbsPose);
  CPPUNIT_TEST(TestSetGetPose);
  CPPUNIT_TEST(TestSetGetConstrainRefSys);
  CPPUNIT_TEST(TestSetGetType);
  CPPUNIT_TEST(TestSetGetBounds);
  CPPUNIT_TEST(TestGetHandleCenter);
  CPPUNIT_TEST(TestShowShadingPlane);
  CPPUNIT_TEST_SUITE_END();
  
  protected:

public:

  void TestFixture();
  void TestConstructor();
  void TestSetGetInput();
  void TestSetGetListener();
  void TestHighlight();
  void TestShow();
  void TestSetGetLength();
  void TestSetGetAbsPose();
  void TestSetGetPose();
  void TestSetGetConstrainRefSys();
  void TestSetGetType();
  void TestSetGetBounds();
  void TestGetHandleCenter();
  void TestShowShadingPlane();
  void CreateTestData();

  albaVMESurface *m_GizmoInputSurface;
  albaVMERoot *m_Root;
  
};

#endif
