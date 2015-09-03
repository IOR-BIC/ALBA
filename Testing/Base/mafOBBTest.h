/*=========================================================================

 Program: MAF2
 Module: mafOBBTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOBBTest_H__
#define __CPP_UNIT_mafOBBTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOBBTest : public mafTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOBBTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructors);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestCopy);
  CPPUNIT_TEST(TestIsValid);
  CPPUNIT_TEST(TestCopyTo);
  CPPUNIT_TEST(TestApplyTransform);
  CPPUNIT_TEST(TestMergeBounds);
  CPPUNIT_TEST(TestIsInside);
  CPPUNIT_TEST(TsetGetDimensions);
  CPPUNIT_TEST(TestGetCenter);
  CPPUNIT_TEST(TestSetCenter);
  CPPUNIT_TEST(TestSetDimensions);
  CPPUNIT_TEST(TestGetWidth);
  CPPUNIT_TEST(TestGetHeight);
  CPPUNIT_TEST(TsetGetDepth);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructors();
  void TestEquals();
  void TestCopy();
  void TestIsValid();
  void TestCopyTo();
  void TestApplyTransform();
  void TestMergeBounds();
  void TestIsInside();
  void TsetGetDimensions();
  void TestGetCenter();
  void TestSetCenter();
  void TestSetDimensions();
  void TestGetWidth();
  void TestGetHeight();
  void TsetGetDepth();

  bool result;
};


#endif
