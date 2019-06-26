/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventBaseTest
 Authors: Daniele Giunchi, Crimi Gianluigi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaEventBaseTest_H__
#define __CPP_UNIT_albaEventBaseTest_H__

#include "albaTest.h"

/** Test for albaEventBase; Use this suite to trace memory problems */
class albaEventBaseTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaEventBaseTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructors);
  CPPUNIT_TEST(TestCopy);
  CPPUNIT_TEST(TestSender);
  CPPUNIT_TEST(TestId);
  CPPUNIT_TEST(TestChannel);
  CPPUNIT_TEST(TestData);
  CPPUNIT_TEST_SUITE_END();
  

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructors();
  void TestCopy();
  void TestSender();
  void TestId();
  void TestChannel();
  void TestData();
};

#endif
