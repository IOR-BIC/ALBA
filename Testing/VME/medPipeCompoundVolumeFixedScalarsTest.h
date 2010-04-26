/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medPipeCompoundVolumeFixedScalarsTest.h,v $
Language:  C++
Date:      $Date: 2010-04-26 14:34:19 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2008
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_medPipeCompoundTest_H__
#define __CPP_UNIT_medPipeCompoundTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class medPipeCompoundVolumeFixedScalarsTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( medPipeCompoundVolumeFixedScalarsTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestAllocation );
  CPPUNIT_TEST( TestCreateSceneNode );
  CPPUNIT_TEST( TestGetCurrentScalarVisualPipe );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestAllocation();
  void TestCreateSceneNode();
  void TestGetCurrentScalarVisualPipe();
};


int main( int argc, char* argv[] )
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
  runner.addTest( medPipeCompoundVolumeFixedScalarsTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
