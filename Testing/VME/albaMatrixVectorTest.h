/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixVectorTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaMatrixVectorTest_H__
#define __CPP_UNIT_albaMatrixVectorTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaMatrixVectorTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaMatrixVectorTest );
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
