/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDataPipeInterpolatorTest_H__
#define __CPP_UNIT_albaDataPipeInterpolatorTest_H__

#include "albaTest.h"

/** Test for albaDataPipeInterpolator; Use this suite to trace memory problems */
class albaDataPipeInterpolatorTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaDataPipeInterpolatorTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestUpdateBounds);
  CPPUNIT_TEST(TestGetCurrentItem);
  CPPUNIT_TEST(TestSetTimeStamp);
  CPPUNIT_TEST(GetMTime);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAccept();
  void TestOnEvent();
  void TestUpdateBounds();
  void TestGetCurrentItem();
  void TestSetTimeStamp();
  void GetMTime();
	

  bool result;
};

#endif

