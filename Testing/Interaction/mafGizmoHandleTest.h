/*=========================================================================

 Program: MAF2
 Module: mafGizmoHandleTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoHandleTest_H__
#define __CPP_UNIT_mafGizmoHandleTest_H__

#include "mafTest.h"

#include "mafVMESurface.h"
#include "mafVMERoot.h"

class mafGizmoHandleTest : public mafTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGizmoHandleTest );
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

  mafVMESurface *m_GizmoInputSurface;
  mafVMERoot *m_Root;
  
};

#endif
