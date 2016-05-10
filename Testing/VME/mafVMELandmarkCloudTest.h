/*=========================================================================

 Program: MAF2
 Module: mafVMELandmarkCloudTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFVMELANDMARKCLOUDTEST_H__
#define __CPP_UNIT_MAFVMELANDMARKCLOUDTEST_H__

#include "mafTest.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h" 

/** Test for mafVMELandmarkCloud */
class mafVMELandmarkCloudTest : public mafTest
{

  public:

		CPPUNIT_TEST_SUITE( mafVMELandmarkCloudTest );
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
