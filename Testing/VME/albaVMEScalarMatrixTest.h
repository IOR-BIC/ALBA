/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEScalarMatrixTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAVMESCALARTEST_H__
#define __CPP_UNIT_ALBAVMESCALARTEST_H__

#include "albaTest.h"

#include <vnl/vnl_matrix.h>

/** Test for albaVMEScalarMatrix.*/
class albaVMEScalarMatrixTest : public albaTest
{
public: 

  // Executed before each test
  void BeforeTest();

  CPPUNIT_TEST_SUITE( albaVMEScalarMatrixTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestVMEScalarFactory );
  CPPUNIT_TEST( TestCopyVMEScalar );
  CPPUNIT_TEST( TestVMEScalarData );
  CPPUNIT_TEST( TestVMEScalarMethods );
  CPPUNIT_TEST( TestAnimatedVMEScalar );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestVMEScalarFactory();
  void TestCopyVMEScalar();
  void TestVMEScalarData();
  void TestVMEScalarMethods();
  void TestAnimatedVMEScalar();

  vnl_matrix<double> in_data;
};

#endif


