/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorVTKTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDataPipeInterpolatorVTKTest_H__
#define __CPP_UNIT_albaDataPipeInterpolatorVTKTest_H__

#include "albaTest.h"

/** Test for albaVTKInterpolator; Use this suite to trace memory problems */
class albaDataPipeInterpolatorVTKTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaDataPipeInterpolatorVTKTest );
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

