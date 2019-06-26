/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpTest_H__
#define __CPP_UNIT_albaOpTest_H__

#include "albaTest.h"

//forward reference
class albaOp;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestSetListener);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestGetType);
  CPPUNIT_TEST(TestCopy);
  CPPUNIT_TEST(TestOpRun);
  CPPUNIT_TEST(TestSetParameters);
  CPPUNIT_TEST(TestOpDo);
  CPPUNIT_TEST(TestOpUndo);
  CPPUNIT_TEST(TestGetGui);
  CPPUNIT_TEST(TestSetGetInput);
  CPPUNIT_TEST(TestSetGetOutput);
  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestCanUndo);
  CPPUNIT_TEST(TestIsInputPreserving);
  CPPUNIT_TEST(TestOkEnabled);
  CPPUNIT_TEST(TestForceStopWithOk);
  CPPUNIT_TEST(TestForceStopWithCancel);
  CPPUNIT_TEST(TestIsCompatible);
  CPPUNIT_TEST(TestGetActions);
  CPPUNIT_TEST(TestSetGetMouse);
  CPPUNIT_TEST(TestTestModeOn);
  CPPUNIT_TEST(TestTestModeOff);
  CPPUNIT_TEST(TestSetGetCanundo);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestSetListener();
  void TestOnEvent();
  void TestGetType();
  void TestCopy();
  void TestOpRun();
  void TestSetParameters();
  void TestOpDo();
  void TestOpUndo();
  void TestGetGui();
  void TestSetGetInput();
  void TestSetGetOutput();
  void TestAccept();
  void TestCanUndo();
  void TestIsInputPreserving();
  void TestOkEnabled();
  void TestForceStopWithOk();
  void TestForceStopWithCancel();
  void TestIsCompatible();
  void TestGetActions();
  void TestSetGetMouse();
  void TestTestModeOn();
  void TestTestModeOff();
  void TestSetGetCanundo();

  bool result;
  albaOp * m_Op;
};

#endif
