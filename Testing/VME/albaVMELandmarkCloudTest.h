/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELandmarkCloudTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAVMELANDMARKCLOUDTEST_H__
#define __CPP_UNIT_ALBAVMELANDMARKCLOUDTEST_H__

#include "albaTest.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h" 

/** Test for albaVMELandmarkCloud */
class albaVMELandmarkCloudTest : public albaTest
{

  public:

		CPPUNIT_TEST_SUITE( albaVMELandmarkCloudTest );
    CPPUNIT_TEST( DynamicAllocationTest );
    CPPUNIT_TEST( AppendLandmarkTest );
    CPPUNIT_TEST( SetLandmarkTest );
    CPPUNIT_TEST( FindLandmarkIndexTest );
    CPPUNIT_TEST( RemoveLandmarkTest );
    CPPUNIT_TEST( GetLandmarkNameTest );
    CPPUNIT_TEST( SetRadiusTest );
    CPPUNIT_TEST( SetSphereResolutionTest );
		CPPUNIT_TEST(DeepCopyTest);
		CPPUNIT_TEST(CopyTreeTest);
		CPPUNIT_TEST(SetDataPointsOnlyTest);
		CPPUNIT_TEST(CreateLMStructureFromDataVectorTest);
		CPPUNIT_TEST(PolydataPointsTest);
		CPPUNIT_TEST(TimeVaryingPointsTest);
		CPPUNIT_TEST(RemoveLandmarkPointsTest);
		CPPUNIT_TEST(ShowLandmarkTest);
		CPPUNIT_TEST(OutputPointSetTest);
		CPPUNIT_TEST(StoreRestoreTest);
		CPPUNIT_TEST(RemoveLandmarkVisibilityTest);
		CPPUNIT_TEST(GetLandmarkTest);
		CPPUNIT_TEST(InvalidLandmarkTest);
		CPPUNIT_TEST(RemoveShownLandmarkTest);
		CPPUNIT_TEST_SUITE_END();

  protected:
    void DynamicAllocationTest();
    void AppendLandmarkTest();
    void SetLandmarkTest();
    void FindLandmarkIndexTest();
    void RemoveLandmarkTest();
    void GetLandmarkNameTest();
    void SetRadiusTest();
    void SetSphereResolutionTest();
		void DeepCopyTest();
		void CopyTreeTest();

		// Tests of the point data handling (formerly inherited from albaVMEPointSet)
		void SetDataPointsOnlyTest();
		void CreateLMStructureFromDataVectorTest();
		void PolydataPointsTest();
		void TimeVaryingPointsTest();
		void RemoveLandmarkPointsTest();
		void ShowLandmarkTest();
		void OutputPointSetTest();
		void StoreRestoreTest();
		void RemoveLandmarkVisibilityTest();
		void GetLandmarkTest();
		void InvalidLandmarkTest();
		void RemoveShownLandmarkTest();

};

#endif
