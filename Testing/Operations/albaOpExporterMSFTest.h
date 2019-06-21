/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMSFTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpExporterMSFTest_H__
#define __CPP_UNIT_albaOpExporterMSFTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpExporterMSFTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpExporterMSFTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestSetFileName );
  CPPUNIT_TEST( TestExportMSF1 );
  CPPUNIT_TEST( TestExportMSF2 );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAccept();
  void TestSetFileName();
  void TestExportMSF1();
  void TestExportMSF2();

  bool m_Result;
};

#endif
