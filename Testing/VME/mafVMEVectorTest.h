/*=========================================================================

 Program: MAF2
 Module: mafVMEVectorTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEVectorTest_H__
#define __CPP_UNIT_mafVMEVectorTest_H__

#include "mafTest.h"

class mafVMEVector;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafVMEVectorTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEVectorTest );
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
  mafVMEVector *m_VmeVector;
};

#endif

