/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpGarbageCollectMSFDirTest.h,v $
Language:  C++
Date:      $Date: 2010-04-20 08:29:34 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafOpGarbageCollectMSFDirTest_H__
#define __CPP_UNIT_mafOpGarbageCollectMSFDirTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


/**
  Class Name: mafOpGarbageCollectMSFDirTest
  Test class for mafOpGarbageCollectMSFDir.
*/
class mafOpGarbageCollectMSFDirTest : public CPPUNIT_NS::TestFixture
{
  /** test suite starting */
  CPPUNIT_TEST_SUITE( mafOpGarbageCollectMSFDirTest );

  /** test OpRun */
  CPPUNIT_TEST( TestOpRun );

  /** test Tree Validation with correct tree */
  CPPUNIT_TEST( TestGarbageCollect );
  
  /** test suite ending  */
  CPPUNIT_TEST_SUITE_END();

  protected:

    /** test OpRun */
    void TestOpRun();

    /** test Tree Validation with correct tree */
    void TestGarbageCollect();
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
  runner.addTest( mafOpGarbageCollectMSFDirTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
