/*=========================================================================

 Program: MAF2
 Module: mafInteractor6DOFCameraMoveTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractor6DOFCameraMoveTest_H__
#define __CPP_UNIT_mafInteractor6DOFCameraMoveTest_H__

#include "mafTest.h"

class mafInteractor6DOFCameraMoveTest : public mafTest
{
  public:

  CPPUNIT_TEST_SUITE( mafInteractor6DOFCameraMoveTest );
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
