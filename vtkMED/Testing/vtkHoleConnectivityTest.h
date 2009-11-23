/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkHoleConnectivityTest.h,v $
Language:  C++
Date:      $Date: 2009-11-23 10:35:18 $
Version:   $Revision: 1.1.2.2 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_vtkHoleConnectivityTEST_H__
#define __CPP_UNIT_vtkHoleConnectivityTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>



class vtkHoleConnectivityTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( vtkHoleConnectivityTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestPrintSelf );
  CPPUNIT_TEST( TestGetClassName );
  CPPUNIT_TEST( TestSetGetPointID );
  CPPUNIT_TEST( TestSetGetPoint );
  CPPUNIT_TEST( TestExecution );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestPrintSelf();
  void TestGetClassName();
  void TestSetGetPointID();
  void TestSetGetPoint();
  void TestExecution();
  
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
  runner.addTest( vtkHoleConnectivityTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif