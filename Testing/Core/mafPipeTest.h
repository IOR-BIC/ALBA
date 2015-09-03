/*=========================================================================

 Program: MAF2
 Module: mafPipeTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeTest_H__
#define __CPP_UNIT_mafPipeTest_H__

#include "mafTest.h"

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafPipeTest : public mafTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafPipeTest );
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

