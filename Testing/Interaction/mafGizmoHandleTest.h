/*=========================================================================

 Program: MAF2
 Module: mafGizmoHandleTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoHandleTest_H__
#define __CPP_UNIT_mafGizmoHandleTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafVMESurface.h"
#include "mafVMERoot.h"

class mafGizmoHandleTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafGizmoHandleTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructor);
  CPPUNIT_TEST(TestSetGetInput);
  CPPUNIT_TEST(TestSetGetListener);
  CPPUNIT_TEST(TestHighlight);
  CPPUNIT_TEST(TestShow);
  CPPUNIT_TEST(TestSetGetLength);
  CPPUNIT_TEST(TestSetGetAbsPose);
  CPPUNIT_TEST(TestSetGetPose);
  CPPUNIT_TEST(TestSetGetConstrainRefSys);
  CPPUNIT_TEST(TestSetGetType);
  CPPUNIT_TEST(TestSetGetBounds);
  CPPUNIT_TEST(TestGetHandleCenter);
  CPPUNIT_TEST(TestShowShadingPlane);
  CPPUNIT_TEST_SUITE_END();
  
  protected:

public:

  void TestFixture();
  void TestConstructor();
  void TestSetGetInput();
  void TestSetGetListener();
  void TestHighlight();
  void TestShow();
  void TestSetGetLength();
  void TestSetGetAbsPose();
  void TestSetGetPose();
  void TestSetGetConstrainRefSys();
  void TestSetGetType();
  void TestSetGetBounds();
  void TestGetHandleCenter();
  void TestShowShadingPlane();
  void CreateTestData();

  mafVMESurface *m_GizmoInputSurface;
  mafVMERoot *m_Root;
  
};


int
main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafGizmoHandleTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
