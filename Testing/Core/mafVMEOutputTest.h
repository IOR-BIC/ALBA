/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEOutputTest_H__
#define __CPP_UNIT_mafVMEOutputTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVMEOutput; Use this suite to trace memory problems */
class mafVMEOutputTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEOutputTest );
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

