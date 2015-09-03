/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputPolylineTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEOutputPolylineTest_H__
#define __CPP_UNIT_mafVMEOutputPolylineTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMEOutputPolylineTest : public mafTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEOutputPolylineTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestUpdate );
  CPPUNIT_TEST( TestCalculateLength );
  CPPUNIT_TEST( TestGetPolylineData );
  CPPUNIT_TEST( TestGetMaterial );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestUpdate();
  void TestCalculateLength();
  void TestGetPolylineData();
  void TestGetMaterial();

  bool m_Result;
};

#endif
