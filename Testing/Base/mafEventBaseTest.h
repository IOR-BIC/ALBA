/*=========================================================================

 Program: MAF2
 Module: mafEventBaseTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafEventBaseTest_H__
#define __CPP_UNIT_mafEventBaseTest_H__

#include "mafTest.h"

/** Test for mafEventBase; Use this suite to trace memory problems */
class mafEventBaseTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafEventBaseTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructors);
  CPPUNIT_TEST(TestCopy);
  CPPUNIT_TEST(TestSender);
  CPPUNIT_TEST(TestId);
  CPPUNIT_TEST(TestChannel);
  CPPUNIT_TEST(TestData);
  CPPUNIT_TEST(TestSkipFlag);
  CPPUNIT_TEST(TestSkipNext);
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
  void TestSkipFlag();
  void TestSkipNext();
};

#endif
