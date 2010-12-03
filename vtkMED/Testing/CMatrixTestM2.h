/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: CMatrixTestM2.h,v $
Language:  C++
Date:      $Date: 2010-12-03 12:07:13 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_CMatrixTestM1_H__
#define __CPP_UNIT_CMatrixTestM1_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class CMatrixTestM2 : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( CMatrixTestM2 );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestGetNumberOfColumns );
  CPPUNIT_TEST( TestGetNumberOfRows );
  CPPUNIT_TEST( TestGetMinSize );
  CPPUNIT_TEST( TestOperator );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestGetNumberOfColumns();
  void TestGetNumberOfRows();
  void TestGetMinSize();
  void TestOperator();
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
  runner.addTest( CMatrixTestM2::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif