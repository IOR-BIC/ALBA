/*=========================================================================

 Program: MAF2
 Module: mafVMEScalarTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEScalarTest_H__
#define __CPP_UNIT_mafVMEScalarTest_H__

#include "mafTest.h"

/** Test for mafVMEScalarMatrix.*/
class mafVMEScalarTest : public mafTest
{
public: 

  // Executed before each test
  void BeforeTest();

  CPPUNIT_TEST_SUITE( mafVMEScalarTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestVMEScalarFactory );
  CPPUNIT_TEST( TestCopyVMEScalar );
  CPPUNIT_TEST( TestVMEScalarData );
  CPPUNIT_TEST( TestAnimatedVMEScalar );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestVMEScalarFactory();
  void TestCopyVMEScalar();
  void TestVMEScalarData();
  void TestAnimatedVMEScalar();
};

#endif
