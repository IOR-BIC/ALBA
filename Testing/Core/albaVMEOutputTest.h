/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEOutputTest_H__
#define __CPP_UNIT_albaVMEOutputTest_H__

#include "albaTest.h"

/** Test for albaVMEOutput; Use this suite to trace memory problems */
class albaVMEOutputTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEOutputTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestSetVME);
  CPPUNIT_TEST(TestGetVME);
  CPPUNIT_TEST(TestGetPose);
  CPPUNIT_TEST(TestGetMatrix);
  CPPUNIT_TEST(TestGetAbsMatrix);
  CPPUNIT_TEST(TestGetAbsPose);
  CPPUNIT_TEST(TestGetVTKData);
  CPPUNIT_TEST(TestGetTimeBounds);
  CPPUNIT_TEST(TestGetlocalTimeBounds);
  CPPUNIT_TEST(TestGetBounds);
  CPPUNIT_TEST(TestGetVMEBounds);
  CPPUNIT_TEST(TestGetVMELocalBounds);
  CPPUNIT_TEST(TestGetTimeStamp);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestGetVME();
  void TestSetVME();
  void TestGetPose();
  void TestGetMatrix();
  void TestGetAbsMatrix();
  void TestGetAbsPose();
  void TestGetVTKData();
  void TestGetTimeBounds();
  void TestGetlocalTimeBounds();
  void TestGetBounds();
  void TestGetVMEBounds();
  void TestGetVMELocalBounds();
  void TestGetTimeStamp();

  bool result;
};

#endif

