/*=========================================================================

 Program: MAF2
 Module: mafOpStackTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpStackTest_H__
#define __CPP_UNIT_mafOpStackTest_H__

#include "mafTest.h"

//forward reference
class mafOpSelect;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOpStackTest : public mafTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpStackTest );
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
