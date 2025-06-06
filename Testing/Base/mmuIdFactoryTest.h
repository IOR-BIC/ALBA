/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmuIdFactoryTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmuIdFactoryTest_H__
#define __CPP_UNIT_mmuIdFactoryTest_H__

#include "albaTest.h"

/** Test for mmuIdFactory; Use this suite to trace memory problems */
class mmuIdFactoryTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mmuIdFactoryTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(StaticAllocationTest);
  CPPUNIT_TEST(DynamicAllocationTest);
  CPPUNIT_TEST(GetNextIdTest);
  CPPUNIT_TEST(GetIdTest);
  CPPUNIT_TEST(GetIdNameTest);
  CPPUNIT_TEST_SUITE_END();  

private:
  void TestFixture();
  void StaticAllocationTest();
  void DynamicAllocationTest();
  void GetNextIdTest();
  void GetIdTest();
  void GetIdNameTest();
};

#endif