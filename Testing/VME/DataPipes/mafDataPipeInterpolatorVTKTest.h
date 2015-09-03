/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorVTKTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDataPipeInterpolatorVTKTest_H__
#define __CPP_UNIT_mafDataPipeInterpolatorVTKTest_H__

#include "mafTest.h"

/** Test for mafVTKInterpolator; Use this suite to trace memory problems */
class mafDataPipeInterpolatorVTKTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafDataPipeInterpolatorVTKTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);

  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestGetCurrentItem);
  CPPUNIT_TEST(TestGetVTKData);
  CPPUNIT_TEST(TestUpdate);
  CPPUNIT_TEST(GetVTKDataPipe);
  
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAccept();
  void TestOnEvent();
  void TestGetCurrentItem();
  void TestGetVTKData();
  void TestUpdate();
  void GetVTKDataPipe();
	

  bool result;
};


#endif

