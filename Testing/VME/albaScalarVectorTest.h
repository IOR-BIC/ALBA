/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaScalarVectorTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaScalarVectorTest_H__
#define __CPP_UNIT_albaScalarVectorTest_H__

#include "albaTest.h"

class albaScalarVector;

/** Test for albaScalarVectorTest; Use this suite to trace memory problems */
class albaScalarVectorTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaScalarVectorTest );
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
  albaScalarVector *m_ScalarVector;
};

#endif

