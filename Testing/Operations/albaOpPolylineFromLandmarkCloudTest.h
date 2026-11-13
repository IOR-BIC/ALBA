/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpPolylineFromLandmarkCloudTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpPolylineFromLandmarkCloudTest_H__
#define __CPP_UNIT_albaOpPolylineFromLandmarkCloudTest_H__

#include "albaTest.h"

class albaVMEStorage;
class albaVMEGroup;
class albaVMELandmarkCloud;

/** Test for albaOpPolylineFromLandmarkCloud.
The fixture has a group with a translation, containing a cloud with another translation and three landmarks. */
class albaOpPolylineFromLandmarkCloudTest : public albaTest
{
public:

	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

	CPPUNIT_TEST_SUITE( albaOpPolylineFromLandmarkCloudTest );
	CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestAccept );
	CPPUNIT_TEST( TestCreatePolyData );
	CPPUNIT_TEST( TestOpRun );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestFixture();
	void TestDynamicAllocation();
	void TestAccept();
	void TestCreatePolyData();
	void TestOpRun();

	albaVMEStorage *m_Storage;
	albaVMEGroup *m_Group;
	albaVMELandmarkCloud *m_Cloud;
};

#endif
