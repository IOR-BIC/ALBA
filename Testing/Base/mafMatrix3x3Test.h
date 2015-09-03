/*=========================================================================

 Program: MAF2
 Module: mafMatrix3x3Test
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafMatrix3x3Test_H__
#define __CPP_UNIT_mafMatrix3x3Test_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafMatrix3x3Test : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafMatrix3x3Test );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestCopy);
  CPPUNIT_TEST(TestZero);
  CPPUNIT_TEST(TestGetVersor);
  CPPUNIT_TEST(TestIdentity);
  CPPUNIT_TEST(TestMultiplyVector);
  CPPUNIT_TEST(TestMultiply);
  CPPUNIT_TEST(TestTranspose);
  CPPUNIT_TEST(TestInvert);
  CPPUNIT_TEST(TestDeterminant);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestCopy();
  void TestZero();
  void TestGetVersor();
  void TestIdentity();
  void TestMultiplyVector();
  void TestMultiply();
  void TestTranspose();
  void TestInvert();
  void TestDeterminant();

  bool result;
};

#endif

