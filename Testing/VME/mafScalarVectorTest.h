/*=========================================================================

 Program: MAF2
 Module: mafScalarVectorTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafScalarVectorTest_H__
#define __CPP_UNIT_mafScalarVectorTest_H__

#include "mafTest.h"

class mafScalarVector;

/** Test for mafScalarVectorTest; Use this suite to trace memory problems */
class mafScalarVectorTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafScalarVectorTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAppendScalar);
  CPPUNIT_TEST(TestGetScalar);
  CPPUNIT_TEST(TestGetScalarVector);
  CPPUNIT_TEST(TestGetNumberOfScalars);
  CPPUNIT_TEST(TestGetNearestScalar);
  CPPUNIT_TEST(TestGetScalarBefore);
  CPPUNIT_TEST(TestGetScalar);
  CPPUNIT_TEST(TestSetScalar);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAppendScalar();
  void TestGetScalarVector();
  void TestGetNumberOfScalars();
  void TestGetNearestScalar();
  void TestGetScalarBefore();
  void TestGetScalar();
  void TestSetScalar();

  bool result;
  mafScalarVector *m_ScalarVector;
};

#endif

