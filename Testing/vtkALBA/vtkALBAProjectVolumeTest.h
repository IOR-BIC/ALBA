/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAProjectVolumeTest
 Authors: Daniele Giunchi, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAProjectSPTEST_H__
#define __CPP_UNIT_vtkALBAProjectSPTEST_H__

#include "albaTest.h"
/**
  Class Name: vtkALBAProjectVolumeTest.
  Test class for vtkALBAProjectVolume.
*/
class vtkALBAProjectVolumeTest : public albaTest
{
  public:

    /** Start Test Suite */
    CPPUNIT_TEST_SUITE( vtkALBAProjectVolumeTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestExecutionProjectionModeToX );
    CPPUNIT_TEST( TestExecutionProjectionModeToY );
 		CPPUNIT_TEST( TestExecutionProjectionModeToZ );
		CPPUNIT_TEST(TestRangeProjectionX);
		CPPUNIT_TEST(TestRangeProjectionY);
		CPPUNIT_TEST(TestRangeProjectionZ);
		/** End Test Suite */
    CPPUNIT_TEST_SUITE_END();

  protected:
 
    /** Allocate and Deallocate filter */
    void TestDynamicAllocation();
    /** Test for projection on X and validate the result */
    void TestExecutionProjectionModeToX();
		/** Test for projection on Y and validate the result */
    void TestExecutionProjectionModeToY();
    /** Test for projection on Z and validate the result */
    void TestExecutionProjectionModeToZ();
		/** Test for projection on X and validate the result */
		void TestRangeProjectionX();
		/** Test for projection on Y and validate the result */
		void TestRangeProjectionY();
		/** Test for projection on Z and validate the result */
		void TestRangeProjectionZ();
		/** Create a new StructuredPoints for Testing*/
		vtkImageData *CreateNewSPWithScalars();
};

#endif
