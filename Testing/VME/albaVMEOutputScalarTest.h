/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputScalarTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEOutputScalarTest_H__
#define __CPP_UNIT_albaVMEOutputScalarTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaVMEOutputScalarTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEOutputScalarTest );
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
