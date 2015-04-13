/*=========================================================================

 Program: MAF2
 Module: mafGUISaveRestorePoseTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNITmafGUISaveRestorePoseTest_H__
#define __CPP_UNITmafGUISaveRestorePoseTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkSphereSource;
class mafVMESurface;

/** mafGUISaveRestorePoseTest */
class mafGUISaveRestorePoseTest : public CPPUNIT_NS::TestFixture
{
public: 
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafGUISaveRestorePoseTest );
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
  
  mafVMESurface *m_VMESphere;
  vtkSphereSource *m_VTKSphere;
};

#endif
