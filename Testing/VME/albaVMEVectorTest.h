/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEVectorTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEVectorTest_H__
#define __CPP_UNIT_albaVMEVectorTest_H__

#include "albaTest.h"

class albaVMEVector;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaVMEVectorTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaVMEVectorTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestSetData);
  CPPUNIT_TEST(TestGetModule);
  CPPUNIT_TEST(TestGetVisualPipe);
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestSetData();
  void TestGetModule();
  void TestGetVisualPipe();

  bool result;
  albaVMEVector *m_VmeVector;
};

#endif

