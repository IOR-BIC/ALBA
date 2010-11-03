/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkWeightedLandmarkTransformTest.h,v $
Language:  C++
Date:      $Date: 2010-11-03 16:33:05 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkWeightedLandmarkTransformTest_H__
#define __CPP_UNIT_vtkWeightedLandmarkTransformTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class mafVMELandmarkCloud;

//------------------------------------------------------------------------------
// Test class for vtkWeightedLandmarkTransform
//------------------------------------------------------------------------------
class vtkWeightedLandmarkTransformTest : public CPPUNIT_NS::TestFixture
{
  public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( vtkWeightedLandmarkTransformTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestUpdate );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestUpdate();
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
  runner.addTest( vtkWeightedLandmarkTransformTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
