/*=========================================================================

 Program: MAF2
 Module: vtkMAFDistanceFilterTest.h
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFDistanceFilterTest_H__
#define __CPP_UNIT_vtkMAFDistanceFilterTest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Test class for vtkMEDImageUnsharpFilter
//------------------------------------------------------------------------------
class vtkMAFDistanceFilterTest : public mafTest
{
  public:
    CPPUNIT_TEST_SUITE( vtkMAFDistanceFilterTest );
 		CPPUNIT_TEST( TestDynamicAllocation );
		CPPUNIT_TEST( TestInput );
		CPPUNIT_TEST( TestGetSet );
    CPPUNIT_TEST( TestFilter_Scalar_Density );
		CPPUNIT_TEST( TestFilter_Vector_Distance );
    CPPUNIT_TEST_SUITE_END();

  protected:

		void TestDynamicAllocation();
		void TestInput();
		void TestGetSet();
		void TestFilter_Scalar_Density();
		void TestFilter_Vector_Distance();
};

#endif
