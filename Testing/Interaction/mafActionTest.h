/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafActionTest.h,v $
Language:  C++
Date:      $Date: 2009-10-29 10:23:11 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafActionTest_H__
#define __CPP_UNIT_mafActionTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafActionTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    /** CPPUNIT fixture: executed before each test */
    void setUp();

    /** CPPUNIT fixture: executed after each test */
    void tearDown();

    CPPUNIT_TEST_SUITE( mafActionTest );
    
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);   
    CPPUNIT_TEST(TestSetGetType);
    CPPUNIT_TEST(TestSetTypeToShared);
    CPPUNIT_TEST(TestSetTypeToExclusive);
    CPPUNIT_TEST(TestBindDevice);
    CPPUNIT_TEST(TestUnBindDevice);
    CPPUNIT_TEST(TestBindInteractor);
    CPPUNIT_TEST(TestUnBindInteractor);
    CPPUNIT_TEST(TestGetDevices);
    
    CPPUNIT_TEST_SUITE_END();
    
  protected:
    
    void TestFixture();
    void TestConstructorDestructor();   
    void TestSetGetType();
    void TestSetTypeToShared();
    void TestSetTypeToExclusive();
    void TestBindDevice();
    void TestUnBindDevice();
    void TestBindInteractor();
    void TestUnBindInteractor();
    void TestGetDevices();
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
  runner.addTest( mafActionTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
