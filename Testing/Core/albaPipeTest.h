/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeTest_H__
#define __CPP_UNIT_albaPipeTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaPipeTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaPipeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(OnEventTest);
  CPPUNIT_TEST(CreateTest);
  CPPUNIT_TEST(GetSetListenerTest);
  CPPUNIT_TEST(SelectTest);
  CPPUNIT_TEST(UpdateProperty);
  CPPUNIT_TEST(GetGui);
  CPPUNIT_TEST(DeleteGui);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void OnEventTest();
  void CreateTest();
  void GetSetListenerTest();
  void SelectTest();
  void UpdateProperty();
  void GetGui();
  void DeleteGui();

  bool result;
};

#endif

