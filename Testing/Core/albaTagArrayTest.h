/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTagArrayTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaTagArrayTest_H__
#define __CPP_UNIT_albaTagArrayTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaTagArrayTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaTagArrayTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructor);
  CPPUNIT_TEST(TestAllCopy);
  CPPUNIT_TEST(TestGetTag);
  CPPUNIT_TEST(TestSetTag);
  CPPUNIT_TEST(TestIsTagPresent);
  CPPUNIT_TEST(TestDeleteTag);
  CPPUNIT_TEST(TestGetNumberOfTags);
  CPPUNIT_TEST(TestGetTagsByType);
  CPPUNIT_TEST(TestGetTagList);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructor();
  void TestAllCopy();
  void TestGetTag();
  void TestSetTag();
  void TestIsTagPresent();
  void TestDeleteTag();
  void TestGetNumberOfTags();
  void TestGetTagsByType();
  void TestGetTagList();

  bool result;
};

#endif
