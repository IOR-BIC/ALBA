/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDeviceButtonsPadTest.h,v $
Language:  C++
Date:      $Date: 2009-05-25 14:51:51 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceButtonsPadTest_H__
#define __CPP_UNIT_mafDeviceButtonsPadTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafDeviceButtonsPadTest : public CPPUNIT_NS::TestFixture
{
 public:

 // CPPUNIT fixture: executed before each test
 void setUp();

 // CPPUNIT fixture: executed after each test
 void tearDown();

 CPPUNIT_TEST_SUITE( mafDeviceButtonsPadTest );
 CPPUNIT_TEST( TestFixture );
 CPPUNIT_TEST( TestConstructorDestructor );
 CPPUNIT_TEST( TestSetGetNumberOfButtons );
 CPPUNIT_TEST( TestSetGetButtonState );
 CPPUNIT_TEST_SUITE_END();

 protected:

   void TestFixture();
   void TestConstructorDestructor();
   void TestSetGetNumberOfButtons();
	  void CreateTestData();
   void TestSetGetButtonState();

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
 runner.addTest( mafDeviceButtonsPadTest::suite());
 runner.run( controller );

 // Print test in a compiler compatible format.
 CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
 outputter.write(); 

 return result.wasSuccessful() ? 0 : 1;
}

#endif
