/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObjectTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaObjectTest_H__
#define __CPP_UNIT_albaObjectTest_H__

#include "albaTest.h"

/** Test for albaObject; Use this suite to trace memory problems */
class albaObjectTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaObjectTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestGetStaticTypeName);
  CPPUNIT_TEST(TestGetTypeName);
  CPPUNIT_TEST(TestIsStaticType);
  CPPUNIT_TEST(TestIsStaticType2);
  CPPUNIT_TEST(TestIsA);
  CPPUNIT_TEST(TestIsA2);
  CPPUNIT_TEST(TestSafeDownCast);
  CPPUNIT_TEST_SUITE_END();


private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestGetStaticTypeName();
  void TestGetTypeName();
  void TestIsStaticType();
  void TestIsStaticType2();
  void TestIsA();
  void TestIsA2();
  void TestSafeDownCast();

};


#endif
