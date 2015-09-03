/*=========================================================================

 Program: MAF2
 Module: vtkMAFFixedCutterTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFFixedCutterTest_H__
#define __CPP_UNIT_vtkMAFFixedCutterTest_H__

#include "mafTest.h"

/**
Class Name: vtkMAFFixedCutterTest.
Test class for vtkMAFFixedCutter.
*/
class vtkMAFFixedCutterTest : public mafTest
{
public:

  /** Start Test Suite */
  CPPUNIT_TEST_SUITE( vtkMAFFixedCutterTest );
  /** Test for dynamic allocation */
  CPPUNIT_TEST( TestDynamicAllocation );
  /** Test of the execution of the filte */
  CPPUNIT_TEST( TestExecute );
  /** End Test Suite */
  CPPUNIT_TEST_SUITE_END();

protected:

  /** Allocate and Deallocate filter */
  void TestDynamicAllocation();
  /** Execution of the filter */
  void TestExecute();

};

#endif
