/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpExporterVTKTest.h,v $
Language:  C++
Date:      $Date: 2009-09-21 13:31:40 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2008 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef CPP_UNIT_mafOpExporterVTKTest_H
#define CPP_UNIT_mafOpExporterVTKTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafOpExporterVTK;


class mafOpExporterVTKTest : public CPPUNIT_NS::TestFixture
{

public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpExporterVTKTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks

  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpExportPolydata );
  CPPUNIT_TEST( TestOpExportPolydataWithApplyABSMatrix );
  CPPUNIT_TEST( TestOpExportVolume );
  CPPUNIT_TEST( TestOpExportMesh );
  CPPUNIT_TEST( TestOpExportPolyline );

  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();

  void TestAccept();
  void TestOpExportPolydata();
  void TestOpExportPolydataWithApplyABSMatrix();
  void TestOpExportVolume();
  void TestOpExportMesh();
  void TestOpExportPolyline();

  bool result;

  mafOpExporterVTK *m_ExporterVTK;
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
  runner.addTest( mafOpExporterVTKTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
