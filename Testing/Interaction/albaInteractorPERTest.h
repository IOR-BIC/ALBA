/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractorPERTest_H__
#define __CPP_UNIT_albaInteractorPERTest_H__

#include "albaTest.h"

class albaInteractorPERTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaInteractorPERTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestEnableSelect);
  CPPUNIT_TEST(TestFlyToMode);
  CPPUNIT_TEST(TestIsInteracting);
  CPPUNIT_TEST(TestStartInteraction);
  CPPUNIT_TEST(TestStopInteraction);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestOnVmeSelected);
  CPPUNIT_TEST(TestSetGetMode);
  CPPUNIT_TEST(TestSetModeToSingleButton);
  CPPUNIT_TEST(TestSetModeToMultiButton);
  CPPUNIT_TEST(TestGetCameraMouseInteractor);
  CPPUNIT_TEST(TestGetCameraInteractor);

  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestEnableSelect();
  void TestFlyToMode();
  void TestIsInteracting();
  void TestStartInteraction();
  void TestStopInteraction();
  void TestOnEvent();
  void TestOnVmeSelected();
  void TestSetGetMode();
  void TestSetModeToSingleButton();
  void TestSetModeToMultiButton();
  void TestGetCameraMouseInteractor();
  void TestGetCameraInteractor();

};

#endif
