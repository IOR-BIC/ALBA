/*=========================================================================

 Program: MAF2
 Module: mafObjectTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafObjectTest_H__
#define __CPP_UNIT_mafObjectTest_H__

#include "mafTest.h"

/** Test for mafObject; Use this suite to trace memory problems */
class mafObjectTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafObjectTest );
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
