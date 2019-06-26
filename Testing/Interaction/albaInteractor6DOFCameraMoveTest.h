/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor6DOFCameraMoveTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractor6DOFCameraMoveTest_H__
#define __CPP_UNIT_albaInteractor6DOFCameraMoveTest_H__

#include "albaTest.h"

class albaInteractor6DOFCameraMoveTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaInteractor6DOFCameraMoveTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestStartInteraction);
  CPPUNIT_TEST(TestOnEventNotHandled);
  CPPUNIT_TEST(TestOnEventOnMchInputChannelWithNULLTracker);
  CPPUNIT_TEST(TestOnEventOnMchInputChannelWithDummyTracker);
  CPPUNIT_TEST(TestOnEventTracker3DMoveOnMchInputChannelWithDummyTracker);
  CPPUNIT_TEST(TestOnEventTracker3DMoveOnMchInputChannelWithDummyTrackerAndNotNULLCamera);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestStartInteraction();
  void TestOnEventNotHandled();
  void TestOnEventOnMchInputChannelWithNULLTracker();
  void TestOnEventOnMchInputChannelWithDummyTracker();
  void TestOnEventTracker3DMoveOnMchInputChannelWithDummyTracker();
  void TestOnEventTracker3DMoveOnMchInputChannelWithDummyTrackerAndNotNULLCamera();
};

#endif
