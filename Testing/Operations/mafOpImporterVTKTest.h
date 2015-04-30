/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVTKTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpImporterVTKTest_H__
#define __CPP_UNIT_mafOpImporterVTKTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafOpImporterVTK;

/** Test for mafOpImporterVTK; Use this suite to trace memory problems */
class mafOpImporterVTKTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpImporterVTKTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  
  //ToDo: add your test here...
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestImportVTKPolyline);
  CPPUNIT_TEST(TestImportVTKSurface);
  CPPUNIT_TEST(TestImportVTKVolume);
  CPPUNIT_TEST(TestImportVTKMesh);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  
  // ToDo: add your test methods here...
  void TestDynamicAllocation();
  void TestAccept();
  void TestImportVTKPolyline();
  void TestImportVTKSurface();
  void TestImportVTKVolume();
  void TestImportVTKMesh();

  bool result;

  mafOpImporterVTK *m_ImporterVTK;
};

#endif
