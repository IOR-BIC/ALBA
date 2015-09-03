/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputScalarMatrixTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEOutputScalarMatrixTest_H__
#define __CPP_UNIT_mafVMEOutputScalarMatrixTest_H__

#include "mafTest.h"
#include <vnl/vnl_matrix.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMEOutputScalarMatrixTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE(mafVMEOutputScalarMatrixTest);
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetScalarData );
  CPPUNIT_TEST( TestGetVTKData_UpdateVTKRepresentation );
  CPPUNIT_TEST( TestUpdate_GetNumberOfRows_GetNumberOfCols );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestGetScalarData();
  void TestGetVTKData_UpdateVTKRepresentation();
  void TestUpdate_GetNumberOfRows_GetNumberOfCols();

	bool m_Result;
  vnl_matrix<double> in_data;
};

#endif
