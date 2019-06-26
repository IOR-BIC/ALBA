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
    CPPUNIT_TEST( SetNumberOfLandmarksTest );
    CPPUNIT_TEST( GetLandmarkNameTest );
    CPPUNIT_TEST( SetRadiusTest );
    CPPUNIT_TEST( SetSphereResolutionTest );
		CPPUNIT_TEST(DeepCopyTest);
		CPPUNIT_TEST(CopyTreeTest);
		CPPUNIT_TEST_SUITE_END();

  protected:
    void DynamicAllocationTest();
    void AppendLandmarkTest();
    void SetLandmarkTest();
    void FindLandmarkIndexTest();
    void RemoveLandmarkTest();
    void SetNumberOfLandmarksTest();
    void GetLandmarkNameTest();
    void SetRadiusTest();
    void SetSphereResolutionTest();
		void DeepCopyTest();
		void CopyTreeTest();

};

#endif
