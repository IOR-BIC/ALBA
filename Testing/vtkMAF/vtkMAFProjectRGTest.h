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

/**
  Class Name: vtkMAFProjectRGTest.
  Test class for vtkMAFProjectRG.
*/
class vtkMAFProjectRGTest : public mafTest
{
  public:
  
    /** Start Test Suite */
    CPPUNIT_TEST_SUITE( vtkMAFProjectRGTest );
    /** Test for dynamic allocation */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test for projection on X */
    CPPUNIT_TEST( TestExecutionProjectionModeToX );
    /** Test for projection on Y */
    CPPUNIT_TEST( TestExecutionProjectionModeToY );
    /** Test for projection on Z */
    CPPUNIT_TEST( TestExecutionProjectionModeToZ );
    /** Test Print */
    CPPUNIT_TEST( TestPrintSelf );
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
    /** Test for Print information*/
    void TestPrintSelf();

};

#endif
