/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafASCIIImporterUtilityTest.h,v $
Language:  C++
Date:      $Date: 2009-11-04 10:02:42 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafASCIIImporterUtilityTest_H__
#define __CPP_UNIT_mafASCIIImporterUtilityTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <iostream>


class mafASCIIImporterUtilityTest : public CPPUNIT_NS::TestFixture
{
public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();
  CPPUNIT_TEST_SUITE( mafASCIIImporterUtilityTest );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( GetNumberOfRowsTest );
  CPPUNIT_TEST( GetNumberOfColsTest );
  CPPUNIT_TEST( GetNumberOfScalarsTest );
  CPPUNIT_TEST( GetScalarTest );
  CPPUNIT_TEST( GetMatrixTest );
  CPPUNIT_TEST( ExtractRowTest );
  CPPUNIT_TEST( ExtractColumnTest );
  CPPUNIT_TEST_SUITE_END();

protected:
    void TestConstructor();
    void GetNumberOfRowsTest();
    void GetNumberOfColsTest();
    void GetNumberOfScalarsTest();
    void GetScalarTest();
    void GetMatrixTest();
    void ExtractRowTest();
    void ExtractColumnTest();
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
  runner.addTest( mafASCIIImporterUtilityTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif // #ifndef __CPP_UNIT_mafASCIIImporterUtilityTest_H__
