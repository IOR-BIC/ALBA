/*=========================================================================

 Program: MAF2
 Module: mafOpTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpTest_H__
#define __CPP_UNIT_mafOpTest_H__

#include "mafTest.h"

//forward reference
class mafOp;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOpTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpTest );
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
  CPPUNIT_TEST(TestCollaborate);
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
  void TestCollaborate();
  void TestTestModeOn();
  void TestTestModeOff();
  void TestSetGetCanundo();

  bool result;
  mafOp * m_Op;
};

#endif
