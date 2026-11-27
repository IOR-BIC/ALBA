/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPositionalEventRouterTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPositionalEventRouterTest_H__
#define __CPP_UNIT_albaPositionalEventRouterTest_H__

#include "albaTest.h"

class albaPositionalEventRouterTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaPositionalEventRouterTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestEnableSelect);
  CPPUNIT_TEST(TestIsInteracting);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestGetCameraTool);
  CPPUNIT_TEST(TestButtonDownPicksAtPosition);
  CPPUNIT_TEST(TestButtonDownPicksOnce);
  CPPUNIT_TEST(TestGenericMouseUsesButtonDownPick);
  CPPUNIT_TEST(TestTransformBehaviorButtons);
  CPPUNIT_TEST(TestEventTracing);
  CPPUNIT_TEST(TestPushPopTool);
  CPPUNIT_TEST(TestToolInteraction);
  CPPUNIT_TEST(TestToolOrder);
  CPPUNIT_TEST(TestBehaviorBeforeTools);
  CPPUNIT_TEST(TestToolGivesDragToCamera);
  CPPUNIT_TEST(TestOneInteractionAtATime);
  CPPUNIT_TEST(TestButtonUpDuringButtonDown);
  CPPUNIT_TEST(TestBehaviorDeletedDuringInteraction);
  CPPUNIT_TEST(TestTransformStoppedAtButtonDown);
  CPPUNIT_TEST(TestOperateOnlyOn);

  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestEnableSelect();
  void TestIsInteracting();
  void TestOnEvent();
  void TestGetCameraTool();
  void TestButtonDownPicksAtPosition();
  void TestButtonDownPicksOnce();
  void TestGenericMouseUsesButtonDownPick();
  void TestTransformBehaviorButtons();
  void TestEventTracing();
  void TestPushPopTool();
  void TestToolInteraction();
  void TestToolOrder();
  void TestBehaviorBeforeTools();
  void TestToolGivesDragToCamera();
  void TestOneInteractionAtATime();
  void TestButtonUpDuringButtonDown();
  void TestBehaviorDeletedDuringInteraction();
  void TestTransformStoppedAtButtonDown();
  void TestOperateOnlyOn();

};

#endif
