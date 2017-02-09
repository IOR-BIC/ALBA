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

#include "mafTest.h"

class mafOpImporterVTK;

/** Test for mafOpImporterVTK; Use this suite to trace memory problems */
class mafOpImporterVTKTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpImporterVTKTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  
  //ToDo: add your test here...
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestImportVTKPolyline);
  CPPUNIT_TEST(TestImportVTKSurface);
  CPPUNIT_TEST(TestImportVTKMesh);
	CPPUNIT_TEST(TestSetFileName);
	CPPUNIT_TEST(TestImportVTKVolumeSP);
	CPPUNIT_TEST(TestImportVTKVolumeRG);
	CPPUNIT_TEST(TestImportVTKPointSet);


  CPPUNIT_TEST_SUITE_END();


private:
  void TestFixture();
  
  // ToDo: add your test methods here...
  void TestDynamicAllocation();
  void TestAccept();
  void TestImportVTKPolyline();
  void TestImportVTKSurface();
  void TestImportVTKMesh();
	void TestSetFileName();
	void TestImportVTKVolumeSP();
	void TestImportVTKVolumeRG();
	void TestImportVTKPointSet();
	bool result;

  mafOpImporterVTK *m_ImporterVTK;
};

#endif
