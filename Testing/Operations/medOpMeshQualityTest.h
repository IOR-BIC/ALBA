/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMeshQualityTest.h,v $
Language:  C++
Date:      $Date: 2010-03-29 08:55:45 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef CPP_UNIT_medOpMeshQualityTest_H
#define CPP_UNIT_medOpMeshQualityTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class medOpMeshQualityTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( medOpMeshQualityTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOpRun();
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
  runner.addTest( medOpMeshQualityTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
