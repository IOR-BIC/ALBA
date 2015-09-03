/*=========================================================================

 Program: MAF2
 Module: mafVectorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVectorTest_H__
#define __CPP_UNIT_mafVectorTest_H__

#include "mafTest.h"

/** Test for mafVector; Use this suite to trace memory problems */
class mafVectorTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVectorTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestGetNumberOfItems);
  CPPUNIT_TEST(TestRemoveItem);
  CPPUNIT_TEST(TestAppendItem);
  CPPUNIT_TEST(TestReplaceItem);
  CPPUNIT_TEST(TestIsItemPresent);
  CPPUNIT_TEST(TestGetSetItem);
  CPPUNIT_TEST(TestFindItem);
  CPPUNIT_TEST(TestPush);
  CPPUNIT_TEST(TestFindAndRemoveItem);
  CPPUNIT_TEST(TestRemoveAllItems);
  CPPUNIT_TEST(TestPop);
  CPPUNIT_TEST(TestVectorAPICombination);
  CPPUNIT_TEST_SUITE_END();


private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetNumberOfItems();
  void TestRemoveItem();
  void TestAppendItem();
  void TestReplaceItem();
  void TestIsItemPresent();
  void TestGetSetItem();
  void TestFindItem();
  void TestPush();
  void TestFindAndRemoveItem();
  void TestRemoveAllItems();
  void TestPop();
  void TestVectorAPICombination();

};


#endif
