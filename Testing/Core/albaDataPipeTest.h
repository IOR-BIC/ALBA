/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDataPipeTest_H__
#define __CPP_UNIT_albaDataPipeTest_H__

#include "albaTest.h"

/** Test for albaDataPipe; Use this suite to trace memory problems */
class albaDataPipeTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaDataPipeTest );
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

