/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpFilterSurfaceTest.h,v $
Language:  C++
Date:      $Date: 2009-10-08 07:26:33 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafOpFilterSurfaceTest_H__
#define __CPP_UNIT_mafOpFilterSurfaceTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkPolyData;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOpFilterSurfaceTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpFilterSurfaceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOnClean );
  CPPUNIT_TEST( TestOnSmooth );
  CPPUNIT_TEST( TestOnDecimate );
  CPPUNIT_TEST( TestOnTriangulate );
  CPPUNIT_TEST( TestOnVtkConnect );
  CPPUNIT_TEST( TestOnStripper );
  CPPUNIT_TEST( TestOnGenerateNormals );
  CPPUNIT_TEST( TestUndo );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAccept();
  void TestOnClean();
  void TestOnSmooth();
  void TestOnDecimate();
  void TestOnTriangulate();
  void TestOnVtkConnect();
  void TestOnStripper();
  void TestOnGenerateNormals();
  void TestUndo();

  void CheckPolydata(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter);
  void CheckNormals(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter);
  bool m_Result;
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
  runner.addTest( mafOpFilterSurfaceTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
