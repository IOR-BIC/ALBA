/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaIndentTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaIndentTest_H__
#define __CPP_UNIT_albaIndentTest_H__

#include "albaTest.h"

/** Test for albaIndent; Use this suite to trace memory problems */
class albaIndentTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaIndentTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestGetTypeName);
  CPPUNIT_TEST(TestGetNextIndent);
  CPPUNIT_TEST(TestOperator_int);
  CPPUNIT_TEST(TestFriend_outputstream);
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetTypeName();
  void TestGetNextIndent();
  void TestOperator_int();
  void TestFriend_outputstream();
};

#endif
