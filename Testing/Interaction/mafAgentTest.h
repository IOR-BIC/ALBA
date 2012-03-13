/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafAgentTest.h,v $
Language:  C++
Date:      $Date: 2008-07-23 11:50:53 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoBoundingBoxTest_H__
#define __CPP_UNIT_mafGizmoBoundingBoxTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafTestAgent;
class mafAgentRouter;

class mafAgentTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafAgentTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructorDestructor );
  CPPUNIT_TEST(  TestAddRemoveObserver );
  CPPUNIT_TEST(  TestRemoveAllObservers );
  CPPUNIT_TEST(  TestHasObservers );
  CPPUNIT_TEST(  TestGetObservers );
  CPPUNIT_TEST(  TestInitialize );
  CPPUNIT_TEST(  TestShutdown );
  CPPUNIT_TEST(  TestIsInitialized );
  CPPUNIT_TEST(  TestSetGetName );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestAddRemoveObserver();
    void TestRemoveObserver();
    void TestRemoveAllObservers();
    void TestHasObservers();
    void TestGetObservers();
    void TestInitialize();
    void TestShutdown();
    void TestIsInitialized();
    void TestSetGetName();

  private:

    mafTestAgent *m_Source;
    mafAgentRouter *m_Router;
    mafTestAgent *m_Sink1;
    mafTestAgent *m_Sink2;
    mafTestAgent *m_Sink3;
    mafTestAgent *m_Sink4;

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
  runner.addTest( mafAgentTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
