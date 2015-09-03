/*=========================================================================

 Program: MAF2
 Module: mafTagItemTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafTagItemTest_H__
#define __CPP_UNIT_mafTagItemTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafTagItemTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafTagItemTest );
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
