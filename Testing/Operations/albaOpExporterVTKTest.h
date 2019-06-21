/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterVTKTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExporterVTKTest_H
#define CPP_UNIT_albaOpExporterVTKTest_H

#include "albaTest.h"

class albaOpExporterVTK;


class albaOpExporterVTKTest : public albaTest
{

public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpExporterVTKTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks

  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpExportPolydata );
  CPPUNIT_TEST( TestOpExportPolydataWithApplyABSMatrix );
  CPPUNIT_TEST( TestOpExportVolume );
  CPPUNIT_TEST( TestOpExportMesh );
  CPPUNIT_TEST( TestOpExportPolyline );

  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();

  void TestAccept();
  void TestOpExportPolydata();
  void TestOpExportPolydataWithApplyABSMatrix();
  void TestOpExportVolume();
  void TestOpExportMesh();
  void TestOpExportPolyline();

  bool result;

  albaOpExporterVTK *m_ExporterVTK;
};

#endif
