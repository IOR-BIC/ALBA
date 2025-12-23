/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttachCameraTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAttachCameraTest_H__
#define __CPP_UNIT_albaAttachCameraTest_H__

#include "albaTest.h"

class vtkDataSet;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;

class albaVMESurface;
class albaVMERoot;

/** Test for albaAttachCamera */
class albaAttachCameraTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaAttachCameraTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestEnableDisableAttachCamera);
  CPPUNIT_TEST(TestSetGetEnableAttachCamera);
  CPPUNIT_TEST(TestSetGetVme);
  CPPUNIT_TEST(TestSetGetListener);
  CPPUNIT_TEST(TestRenderStuff);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestRenderStuff();
  void TestConstructorDestructor();
  void TestEnableDisableAttachCamera();
  void TestSetGetEnableAttachCamera();
  void TestSetGetVme();
  void TestSetGetListener();  

  void RenderVMESurface( albaVMESurface *vme );
  void CreateTestData();
   
  albaVMESurface *m_TestSurface;
  albaVMERoot *m_Root;
};

#endif
