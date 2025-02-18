/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustomTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDataPipeCustomTest_H__
#define __CPP_UNIT_albaDataPipeCustomTest_H__

#include "albaTest.h"

/** Test for albaDataPipeCustom; Use this suite to trace memory problems */
class albaDataPipeCustomTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaDataPipeCustomTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);

  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestGetVTKData);
  CPPUNIT_TEST(TestGetVTKDataPipe);
  CPPUNIT_TEST(TestUpdate);
  CPPUNIT_TEST(TestUpdateBounds);
  CPPUNIT_TEST(TestSetInput);
  CPPUNIT_TEST(TestSetNthInput);
  
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestOnEvent();
  void TestGetVTKData();
  void TestGetVTKDataPipe();
  void TestUpdate();
  void TestUpdateBounds();
  void TestSetInput();
  void TestSetNthInput();
};

#endif

