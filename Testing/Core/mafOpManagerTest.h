/*=========================================================================

 Program: MAF2
 Module: mafOpManagerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpManagerTest_h__
#define __CPP_UNIT_mafOpManagerTest_h__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

#include <wx/config.h>
#include <wx/fileconf.h>

class mafOpManager;

/** mafOpManagerTest:
Test class for mafOpManager
*/
//------------------------------------------------------------------------------
class mafOpManagerTest: public CPPUNIT_NS::TestFixture
  //------------------------------------------------------------------------------
{

public:

  /** CPPUNIT fixture: executed before each test.*/
  void setUp();
  /* CPPUNIT fixture: executed after each test.*/
  void tearDown();
  /** Test suite begin. */
  CPPUNIT_TEST_SUITE( mafOpManagerTest );

  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestOpAdd );
  CPPUNIT_TEST( TestVmeSelected );
  CPPUNIT_TEST( TestOpRun1 );
  CPPUNIT_TEST( TestOpRun2 );
  CPPUNIT_TEST( TestOpRun3 );
  CPPUNIT_TEST( TestOpExec );
  CPPUNIT_TEST( TestOpSelect );
  CPPUNIT_TEST( TestRunning );
  CPPUNIT_TEST( TestClearUndoStack );
  CPPUNIT_TEST( TestForceStopWithOk );
  CPPUNIT_TEST( TestForceStopWithCancel );
  CPPUNIT_TEST( TestStopCurrentOperation );
  CPPUNIT_TEST( TestGetRunningOperation );
  CPPUNIT_TEST( TestGetOperationById );
  CPPUNIT_TEST( TestCollaborate );
  CPPUNIT_TEST( TestSetMafUser );
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestOpAdd();
  void TestVmeSelected();
  void TestOpRun1();
  void TestOpRun2();
  void TestOpRun3();
  void TestOpExec();
  void TestOpSelect();
  void TestRunning();
  void TestClearUndoStack();
  void TestForceStopWithOk();
  void TestForceStopWithCancel();
  void TestStopCurrentOperation();
  void TestGetRunningOperation();
  void TestGetOperationById();
  void TestCollaborate();
  void TestSetMafUser();

  mafOpManager *m_OpManager;

};

int 
main( int argc, char* argv[] )
{

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that collects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafOpManagerTest::suite() );
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;

}

#endif // #ifndef __CPP_UNIT_mafOpManagerTest_h__