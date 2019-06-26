/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTagItemTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaTagItemTest_H__
#define __CPP_UNIT_albaTagItemTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaTagItemTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaTagItemTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructor);
  CPPUNIT_TEST(TestAllCopy);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestTagName);
  CPPUNIT_TEST(TestTagValue);
  CPPUNIT_TEST(TestRemoveValue);
  CPPUNIT_TEST(TestTagComponent);
  CPPUNIT_TEST(TestTagType);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructor();
  void TestAllCopy();
  void TestEquals();
  void TestTagName();
  void TestTagValue();
  void TestRemoveValue();
  void TestTagComponent();
  void TestTagType();

  bool result;
};

#endif
