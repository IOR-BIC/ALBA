/*=========================================================================

 Program: MAF2
 Module: mafOpExporterVTKTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpExporterVTKTest_H
#define CPP_UNIT_mafOpExporterVTKTest_H

#include "mafTest.h"

class mafOpExporterVTK;


class mafOpExporterVTKTest : public mafTest
{

public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpExporterVTKTest );
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

  mafOpExporterVTK *m_ExporterVTK;
};

#endif
