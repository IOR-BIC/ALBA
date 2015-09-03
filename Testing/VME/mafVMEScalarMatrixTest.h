/*=========================================================================

 Program: MAF2
 Module: mafVMEScalarMatrixTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFVMESCALARTEST_H__
#define __CPP_UNIT_MAFVMESCALARTEST_H__

#include "mafTest.h"

#include <vnl/vnl_matrix.h>

/** Test for mafVMEScalarMatrix.*/
class mafVMEScalarMatrixTest : public mafTest
{
public: 

  // Executed before each test
  void BeforeTest();

  CPPUNIT_TEST_SUITE( mafVMEScalarMatrixTest );
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


