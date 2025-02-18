/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustomProberTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDataPipeCustomProberTest_H__
#define __CPP_UNIT_albaDataPipeCustomProberTest_H__

#include "albaTest.h"

/** Test for albaDataPipeCustom; Use this suite to trace memory problems */
class albaDataPipeCustomProberTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaDataPipeCustomProberTest );
//   CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
//   CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestExecute);
//   CPPUNIT_TEST(TestSetSurface);
//   CPPUNIT_TEST(TestSetVolume);
//   CPPUNIT_TEST(TestSetGet);  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestExecute();
  void TestSetSurface();
  void TestSetVolume();
  void TestSetGet();
};

#endif

