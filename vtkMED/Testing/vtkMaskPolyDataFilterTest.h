/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMaskPolyDataFilterTest.h,v $
Language:  C++
Date:      $Date: 2007-04-17 11:04:58 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef CPP_UNIT_vtkMaskPolyDataFilterTEST_H
#define CPP_UNIT_vtkMaskPolyDataFilterTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class vtkMaskPolyDataFilterTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( vtkMaskPolyDataFilterTest );
  CPPUNIT_TEST( TestStructuredPoints );
	CPPUNIT_TEST( TestRectilinearGrid );
  CPPUNIT_TEST_SUITE_END();

  protected:
		void TestStructuredPoints();
    void TestRectilinearGrid();
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
  runner.addTest( vtkMaskPolyDataFilterTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
