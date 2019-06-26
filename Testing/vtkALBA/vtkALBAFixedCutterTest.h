/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixedCutterTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAFixedCutterTest_H__
#define __CPP_UNIT_vtkALBAFixedCutterTest_H__

#include "albaTest.h"

/**
Class Name: vtkALBAFixedCutterTest.
Test class for vtkALBAFixedCutter.
*/
class vtkALBAFixedCutterTest : public albaTest
{
public:

  /** Start Test Suite */
  CPPUNIT_TEST_SUITE( vtkALBAFixedCutterTest );
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
