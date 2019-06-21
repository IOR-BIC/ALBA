/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpStackTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpStackTest_H__
#define __CPP_UNIT_albaOpStackTest_H__

#include "albaTest.h"

//forward reference
class albaOpSelect;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpStackTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpStackTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestIsEmpty );
  CPPUNIT_TEST( TestPush );
  CPPUNIT_TEST( TestPop );
  CPPUNIT_TEST( TestClear );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestIsEmpty();
  void TestPush();
  void TestPop();
  void TestClear();

  bool result;
};

#endif
