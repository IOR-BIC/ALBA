/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpExporterLandmarkTest.h,v $
Language:  C++
Date:      $Date: 2008-04-28 08:59:56 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef CPP_UNIT_medOpExporterLandmarkTEST_H
#define CPP_UNIT_medOpExporterLandmarkTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class medOpExporterLandmarkTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( medOpExporterLandmarkTest );
  //CPPUNIT_TEST( TestOnVmeRawMotionData ); ///< this test is leaked for vmerawmotiondata
	CPPUNIT_TEST( TestOnLandmarkImporter);
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestOnVmeRawMotionData();
		void TestOnLandmarkImporter();
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
  runner.addTest( medOpExporterLandmarkTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
