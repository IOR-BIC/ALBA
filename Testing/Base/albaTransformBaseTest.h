/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformBaseTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaTransformBaseTest_H__
#define __CPP_UNIT_albaTransformBaseTest_H__

#include "albaTest.h"

/** Test for albaTransformBase; Use this suite to trace memory problems */
class albaTransformBaseTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaTransformBaseTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestCopyConstructor);
  CPPUNIT_TEST(TestGetMatrix);
	CPPUNIT_TEST(TestTimeStamp);
	CPPUNIT_TEST(TestModifiedTime);
	CPPUNIT_TEST(TestUpdateTime);
	CPPUNIT_TEST(TestGetVTKTransform);
	CPPUNIT_TEST(TestInternalTransformPoint);
 
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestCopyConstructor();
	void TestGetMatrix();
	void TestTimeStamp();
	void TestModifiedTime();
	void TestUpdateTime();
	void TestGetVTKTransform();
	void TestInternalTransformPoint();
};


#endif
