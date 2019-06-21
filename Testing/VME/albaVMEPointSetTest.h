/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPointSetTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEPointSetTest_H__
#define __CPP_UNIT_albaVMEPointSetTest_H__

#include "albaTest.h"

class albaVMEPointSet;

/** Test for albaVMEPointSet; Use this suite to trace memory problems */
class albaVMEPointSetTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEPointSetTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST( TestAppendPoint );
  CPPUNIT_TEST( TestRemovePoint );
  CPPUNIT_TEST( TestSetPoint );
  CPPUNIT_TEST( TestGetPoint );
  CPPUNIT_TEST( TestGetNumberOfPoints );
  CPPUNIT_TEST( TestSetNumberOfPoints );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestSetData();
  void TestAppendPoint();
  void TestRemovePoint();
  void TestSetPoint();
  void TestGetPoint();
  void TestGetNumberOfPoints();
  void TestSetNumberOfPoints();

  bool result;

  albaVMEPointSet *m_PointSetVme;

};

#endif
