/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUISaveRestorePoseTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNITalbaGUISaveRestorePoseTest_H__
#define __CPP_UNITalbaGUISaveRestorePoseTest_H__

#include "albaTest.h"

class vtkSphereSource;
class albaVMESurface;

/** albaGUISaveRestorePoseTest */
class albaGUISaveRestorePoseTest : public albaTest
{
public: 
  /** Executed before each test */
  void BeforeTest();

  /** Executed after each test */
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGUISaveRestorePoseTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestStorePose);
  CPPUNIT_TEST(TestRestorePose);
  CPPUNIT_TEST(TestRemovePose);
  CPPUNIT_TEST(TestStorePoseHelper);
  CPPUNIT_TEST(TestRestorePoseHelper);
  CPPUNIT_TEST(TestRemovePoseHelper);
  CPPUNIT_TEST_SUITE_END();

  
private:

  void TestFixture();
  void TestConstructorDestructor();
  void TestStorePose();
  void TestRestorePose();
  void TestRemovePose();  
  void TestStorePoseHelper();
  void TestRestorePoseHelper();
  void TestRemovePoseHelper();
  
  albaVMESurface *m_VMESphere;
  vtkSphereSource *m_VTKSphere;
};

#endif
