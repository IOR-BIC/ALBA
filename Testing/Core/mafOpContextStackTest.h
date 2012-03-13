/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpContextStackTest.h,v $
Language:  C++
Date:      $Date: 2008-02-19 14:50:19 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef CPP_UNIT_mafOpContextStackTEST_H
#define CPP_UNIT_mafOpContextStackTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class mafOpContextStackTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( mafOpContextStackTest );
  CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( Test_Caller_Clear_Push_Pop );
  CPPUNIT_TEST( Test_UndoClear_UndoPush_UndoPop_UndoIsEmpty );
  CPPUNIT_TEST( Test_RedoClear_RedoPush_RedoPop_RedoIsEmpty );
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void Test_Caller_Clear_Push_Pop();
    void Test_UndoClear_UndoPush_UndoPop_UndoIsEmpty();
    void Test_RedoClear_RedoPush_RedoPop_RedoIsEmpty();
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
  runner.addTest( mafOpContextStackTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif