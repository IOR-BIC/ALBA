/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractorPERTest_H__
#define __CPP_UNIT_mafInteractorPERTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafInteractorPERTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  /** CPPUNIT fixture: executed before each test */
  void setUp();

  /** CPPUNIT fixture: executed after each test */
  void tearDown();

  CPPUNIT_TEST_SUITE( mafInteractorPERTest );
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
  CPPUNIT_TEST(TestLinkCameraAdd);
  CPPUNIT_TEST(TestLinkCameraRemove);
  CPPUNIT_TEST(TestLinkCameraRemoveAll);
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
  void TestLinkCameraAdd();
  void TestLinkCameraRemove();
  void TestLinkCameraRemoveAll();
  void TestGetCameraMouseInteractor();
  void TestGetCameraInteractor();

};

#endif
