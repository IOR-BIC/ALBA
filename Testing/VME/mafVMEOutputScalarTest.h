/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputScalarTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEOutputScalarTest_H__
#define __CPP_UNIT_mafVMEOutputScalarTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMEOutputScalarTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEOutputScalarTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetScalarData );
  CPPUNIT_TEST( TestGetVTKData_TestUpdateVTKRepresentation );
  CPPUNIT_TEST( TestUpdate_GetScalarDataString );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestGetScalarData();
  void TestGetVTKData_TestUpdateVTKRepresentation();
  void TestUpdate_GetScalarDataString();

	bool m_Result;
};

#endif
