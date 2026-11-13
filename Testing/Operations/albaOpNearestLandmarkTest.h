/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpNearestLandmarkTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpNearestLandmarkTest_H__
#define __CPP_UNIT_albaOpNearestLandmarkTest_H__

#include "albaTest.h"

class albaVME;
class albaVMEStorage;
class albaVMELandmarkCloud;
class albaVMELandmark;
class albaVMESurfaceParametric;
class vtkPolyData;

/** Test for albaOpNearestLandmark.
The fixture has a landmark at (30,0,0) and a parametric sphere of radius 10 centered in (5,0,0):
the closest point of the sphere to the landmark is (15,0,0). */
class albaOpNearestLandmarkTest : public albaTest
{
public:

	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

	CPPUNIT_TEST_SUITE( albaOpNearestLandmarkTest );
	CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestAccept );
	CPPUNIT_TEST( TestLandmarkAndSurfaceAccept );
	CPPUNIT_TEST( TestCreateClosestPoint );
	CPPUNIT_TEST( TestCreateClosestPointUniqueName );
	CPPUNIT_TEST( TestOpRunWithLandmarkInput );
	CPPUNIT_TEST( TestOpRunWithOtherInput );
	CPPUNIT_TEST( TestOpRunCancel );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestFixture();
	void TestDynamicAllocation();
	void TestAccept();
	void TestLandmarkAndSurfaceAccept();
	void TestCreateClosestPoint();
	void TestCreateClosestPointUniqueName();
	void TestOpRunWithLandmarkInput();
	void TestOpRunWithOtherInput();
	void TestOpRunCancel();

	/** Run the operation on input, answering the VME choices with the given landmark and surface (NULL to cancel) */
	void RunOperation(albaVME *input, albaVME *landmark, albaVME *surface, int expectedChoices);

	/** Check that the last landmark of the cloud is at (15,0,0) with the given name */
	void CheckLastLandmark(const char *expectedName);

	albaVMEStorage *m_Storage;
	albaVMELandmarkCloud *m_Cloud;
	albaVMELandmark *m_Landmark;
	albaVMESurfaceParametric *m_Surface;
};

#endif
