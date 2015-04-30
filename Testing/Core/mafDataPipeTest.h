/*=========================================================================

 Program: MAF2
 Module: mafDataPipeTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDataPipeTest_H__
#define __CPP_UNIT_mafDataPipeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafDataPipe; Use this suite to trace memory problems */
class mafDataPipeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafDataPipeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestUpdateBounds);
  CPPUNIT_TEST(TestUpdate);
  CPPUNIT_TEST(TestGetBounds);
  CPPUNIT_TEST(TestGetVTKData);
  CPPUNIT_TEST(TestSetGetTimeStamp);
  CPPUNIT_TEST(TestGetMTime);
  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestMakeACopy);
	CPPUNIT_TEST(TestDeepCopy);
	CPPUNIT_TEST(TestSetGetVME);
	CPPUNIT_TEST(TestSetGetDependOnPose);
	CPPUNIT_TEST(TestSetGetDependOnVMETime);
	CPPUNIT_TEST(TestSetGetDependOnAbsPose);
	CPPUNIT_TEST(TestPrint);
	CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
	void TestUpdateBounds();
	void TestUpdate();
	void TestGetBounds();
	void TestGetVTKData();
	void TestSetGetTimeStamp();
	void TestGetMTime();
	void TestAccept();
	void TestMakeACopy();
	void TestDeepCopy();
	void TestSetGetVME();
	void TestSetGetDependOnPose();
	void TestSetGetDependOnVMETime();
	void TestSetGetDependOnAbsPose();
	void TestPrint();
	void TestOnEvent();

  bool result;
};

#endif

