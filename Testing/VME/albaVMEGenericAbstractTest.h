/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGenericAbstractTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEGenericAbstractTest_H__
#define __CPP_UNIT_albaVMEGenericAbstractTest_H__

#include "albaTest.h"

/** Test for albaVMEGenericAbstract; Use this suite to trace memory problems */
class albaVMEGenericAbstractTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEGenericAbstractTest );
	CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructor);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestShallowCopy);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestReparentTo);
  CPPUNIT_TEST(TestGetMatrixVector);
  CPPUNIT_TEST(TestSetMatrix);
  CPPUNIT_TEST(TestGetDataVector);
  CPPUNIT_TEST(TestGetDataTimeStamps);
  CPPUNIT_TEST(TestGetMatrixTimeStamps);
  CPPUNIT_TEST(TestGetLocalTimeStamps);
  CPPUNIT_TEST(TestGetLocalTimeBounds);
  CPPUNIT_TEST(TestIsAnimated);
  CPPUNIT_TEST(TestIsDataAvailable);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAllConstructor();
  void TestDeepCopy();
  void TestShallowCopy();
  void TestEquals();
  void TestReparentTo();
  void TestGetMatrixVector();
  void TestSetMatrix();
  void TestGetDataVector();
  void TestGetDataTimeStamps();
  void TestGetMatrixTimeStamps();
  void TestGetLocalTimeStamps();
  void TestGetLocalTimeBounds();
  void TestIsAnimated();
  void TestIsDataAvailable();

  bool result;
};

#endif