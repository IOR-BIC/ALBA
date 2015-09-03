/*=========================================================================

 Program: MAF2
 Module: mafVMEPointSetTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEPointSetTest_H__
#define __CPP_UNIT_mafVMEPointSetTest_H__

#include "mafTest.h"

class mafVMEPointSet;

/** Test for mafVMEPointSet; Use this suite to trace memory problems */
class mafVMEPointSetTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEPointSetTest );
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

  mafVMEPointSet *m_PointSetVme;

};

#endif
