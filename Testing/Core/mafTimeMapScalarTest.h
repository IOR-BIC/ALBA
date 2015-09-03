/*=========================================================================

 Program: MAF2
 Module: mafTimeMapScalarTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafTimeMapScalarTest_H__
#define __CPP_UNIT_mafTimeMapScalarTest_H__

#include "mafTest.h"

class mafTestSVector;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafTimeMapScalarTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafTimeMapScalarTest );
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

  void FillScalarVector(double *t, double *s, int n);

  mafTestSVector *m_SVector;
  bool result;
};


#endif
