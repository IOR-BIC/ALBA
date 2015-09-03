/*=========================================================================

 Program: MAF2
 Module: mafTagArrayTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafTagArrayTest_H__
#define __CPP_UNIT_mafTagArrayTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafTagArrayTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafTagArrayTest );
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
