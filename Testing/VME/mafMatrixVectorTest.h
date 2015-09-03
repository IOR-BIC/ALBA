/*=========================================================================

 Program: MAF2
 Module: mafMatrixVectorTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafMatrixVectorTest_H__
#define __CPP_UNIT_mafMatrixVectorTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafMatrixVectorTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafMatrixVectorTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetNumberOfMatrices );
  CPPUNIT_TEST( TestGetNearestMatrix );
  CPPUNIT_TEST( TestGetMatrixBefore );
  CPPUNIT_TEST( TestGetMatrix );
  CPPUNIT_TEST( TestSetMatrix );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAppendKeyMatrix();
  void TestGetNumberOfMatrices();
  void TestGetNearestMatrix();
  void TestGetMatrixBefore();
  void TestGetMatrix();
  void TestSetMatrix();
};

#endif
