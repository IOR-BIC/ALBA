/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoLandmarkImporterWSTest.h,v $
Language:  C++
Date:      $Date: 2007-03-05 16:44:13 $
Version:   $Revision: 1.1 $
Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef CPP_UNIT_mmoLandmarkImporterWSTEST_H
#define CPP_UNIT_mmoLandmarkImporterWSTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class mmoLandmarkImporterWSTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( mmoLandmarkImporterWSTest );
  CPPUNIT_TEST( TestWSImporter );
  CPPUNIT_TEST( TestVisibility );
  CPPUNIT_TEST( TestCoordinates );
 
  CPPUNIT_TEST_SUITE_END();

  protected:
    void TestWSImporter();
    void TestVisibility();
    void TestCoordinates();
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
  runner.addTest( mmoLandmarkImporterWSTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}



#endif
