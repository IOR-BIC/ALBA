/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVTKXMLTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpImporterVTKXMLTest_H
#define CPP_UNIT_mafOpImporterVTKXMLTest_H

#include "mafTest.h"

class mafOpImporterVTKXMLTest : public mafTest
{

public:
  // Executed before each test
  void BeforeTest();

  CPPUNIT_TEST_SUITE( mafOpImporterVTKXMLTest );
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
