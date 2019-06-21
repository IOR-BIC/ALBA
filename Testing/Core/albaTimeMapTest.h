/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTimeMapTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaTimeMapTest_H__
#define __CPP_UNIT_albaTimeMapTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaTimeMapTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaTimeMapTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestAllocation);
  CPPUNIT_TEST(TestNumberOfItems);
  CPPUNIT_TEST(TestOrdering);
  CPPUNIT_TEST(TestFindByIndex);
  CPPUNIT_TEST(TestInsertion);
  CPPUNIT_TEST(TestSortIntegrity);
  CPPUNIT_TEST(TestFindAlgorithm);
  CPPUNIT_TEST(TestSearchOutOfBounds);
  CPPUNIT_TEST(TestItemsRemoval);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestAllocation();
  void TestNumberOfItems();
  void TestOrdering();
  void TestFindByIndex();
  void TestInsertion();
  void TestSortIntegrity();
  void TestFindAlgorithm();
  void TestSearchOutOfBounds();
  void TestItemsRemoval();

  bool result;
};


#endif
