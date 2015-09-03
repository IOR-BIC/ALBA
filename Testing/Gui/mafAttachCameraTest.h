/*=========================================================================

 Program: MAF2
 Module: mafAttachCameraTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAttachCameraTest_H__
#define __CPP_UNIT_mafAttachCameraTest_H__

#include "mafTest.h"

class vtkDataSet;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;

class mafVMESurface;
class mafVMERoot;

/** Test for mafAttachCamera */
class mafAttachCameraTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafAttachCameraTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestEnableDisableAttachCamera);
  CPPUNIT_TEST(TestSetGetEnableAttachCamera);
  CPPUNIT_TEST(TestSetGetVme);
  CPPUNIT_TEST(TestSetGetStartingMatrix);
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
  void TestSetGetStartingMatrix();
  void TestSetGetListener();  

  void CreateRenderStuff();
  void RenderVMESurface( mafVMESurface *vme );
  void CreateTestData();
   
  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;
  
  mafVMESurface *m_TestSurface;
  mafVMERoot *m_Root;

};

#endif
