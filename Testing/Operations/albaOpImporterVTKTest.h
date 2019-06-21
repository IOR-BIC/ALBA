/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVTKTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpImporterVTKTest_H__
#define __CPP_UNIT_albaOpImporterVTKTest_H__

#include "albaTest.h"

class albaOpImporterVTK;

/** Test for albaOpImporterVTK; Use this suite to trace memory problems */
class albaOpImporterVTKTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpImporterVTKTest );
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

  albaOpImporterVTK *m_ImporterVTK;
};

#endif
