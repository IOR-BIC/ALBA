/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVTKXMLTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpImporterVTKXMLTest_H
#define CPP_UNIT_albaOpImporterVTKXMLTest_H

#include "albaTest.h"

class albaOpImporterVTKXMLTest : public albaTest
{

public:
  // Executed before each test
  void BeforeTest();

  CPPUNIT_TEST_SUITE( albaOpImporterVTKXMLTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestSetFileName );
  CPPUNIT_TEST( TestImportVTKPolylineGraph );
  CPPUNIT_TEST( TestImportVTKVolumeSP );
  CPPUNIT_TEST( TestImportVTKVolumeRG );
  CPPUNIT_TEST( TestImportVTKSurface );
  CPPUNIT_TEST( TestImportVTKMesh );
  CPPUNIT_TEST( TestImportVTKPointSet );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestAccept();
  void TestSetFileName();
  void TestImportVTKPolylineGraph();
  void TestImportVTKVolumeSP();
  void TestImportVTKVolumeRG();
  void TestImportVTKSurface();
  void TestImportVTKMesh();
  void TestImportVTKPointSet();
};

#endif
