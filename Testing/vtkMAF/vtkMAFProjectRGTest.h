/*=========================================================================

 Program: MAF2
 Module: vtkMAFProjectRGTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFProjectRGTEST_H__
#define __CPP_UNIT_vtkMAFProjectRGTEST_H__

#include "mafTest.h"

class vtkRectilinearGrid;

/**
  Class Name: vtkMAFProjectRGTest.
  Test class for vtkMAFProjectRG.
*/
class vtkMAFProjectRGTest : public mafTest
{
  public:
  
    /** Start Test Suite */
    CPPUNIT_TEST_SUITE( vtkMAFProjectRGTest );
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
		/** Create a new RectilinearGrid for Testing*/
		vtkRectilinearGrid * CreateNewRGWithScalars();
};

#endif
