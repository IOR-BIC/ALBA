/*=========================================================================

 Program: MAF2
 Module: mafDataPipeInterpolatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDataPipeInterpolatorTest_H__
#define __CPP_UNIT_mafDataPipeInterpolatorTest_H__

#include "mafTest.h"

/** Test for mafDataPipeInterpolator; Use this suite to trace memory problems */
class mafDataPipeInterpolatorTest : public mafTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafDataPipeInterpolatorTest );
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

