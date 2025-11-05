/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVersionTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVersionTest_H__
#define __CPP_UNIT_albaVersionTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaVersionTest : public albaTest
{
public: 

	// CPPUNIT test suite
	CPPUNIT_TEST_SUITE( albaVersionTest );
	CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST(TestStaticAllocation);
	CPPUNIT_TEST(TestDynamicAllocation);
	CPPUNIT_TEST(TestGetALBAVersion);
	CPPUNIT_TEST(TestGetALBAMajorVersion);
	CPPUNIT_TEST(TestGetALBAMinorVersion);
	CPPUNIT_TEST(TestGetALBASourceVersion);
	CPPUNIT_TEST_SUITE_END();

private:
	void TestFixture();
	void TestStaticAllocation();
	void TestDynamicAllocation();
	void TestGetALBAVersion();
	void TestGetALBAMajorVersion();
	void TestGetALBAMinorVersion();
	void TestGetALBASourceVersion();
};

#endif
