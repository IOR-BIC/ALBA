/*=========================================================================

 Program: MAF2
 Module: mafNodeManagerTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafNodeManagerTest_h__
#define __CPP_UNIT_mafNodeManagerTest_h__

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

/** mafNodeManagerTest:
Test class for mafNodeManager
*/
//------------------------------------------------------------------------------
class mafNodeManagerTest: public CPPUNIT_NS::TestFixture, public mafObserver
//------------------------------------------------------------------------------
{

public:

  /** CPPUNIT fixture: executed before each test.*/
  void setUp();
  /* CPPUNIT fixture: executed after each test.*/
  void tearDown();
  /** Test suite begin. */
  CPPUNIT_TEST_SUITE( mafNodeManagerTest );

  CPPUNIT_TEST( MSFNewTest );

  CPPUNIT_TEST( VmeAddRemoveTest );

  CPPUNIT_TEST( TimeGetBoundsTest );

  CPPUNIT_TEST( SetGetFileNameTest );

  CPPUNIT_TEST( GetRootTest );

  CPPUNIT_TEST_SUITE_END();

  void OnEvent(mafEventBase *maf_event);

protected:

  void MSFNewTest();

  // Other MSF methods are no longer implemented (commented code, see mafNodeManager.cpp) 

  /** test add and remove vme */
  void VmeAddRemoveTest();

  /** test get time bounds */
  void TimeGetBoundsTest();

  /** test get and set file name */
  void SetGetFileNameTest();

  /** test get root */
  void GetRootTest();		

  int m_EventID;
  mafNode *m_Node;
  wxFileConfig *m_Config;
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
  runner.addTest( mafNodeManagerTest::suite() );
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;

}

#endif // #ifndef __CPP_UNIT_mafNodeManagerTest_h__