/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFDOFMatrixTest.h,v $
Language:  C++
Date:      $Date: 2009-03-25 15:01:42 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFRemoveCellsFilterTEST_H__
#define __CPP_UNIT_vtkMAFRemoveCellsFilterTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkMAFDOFMatrix;

class vtkMAFDOFMatrixTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFDOFMatrixTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST(TestDeepCopy);
    CPPUNIT_TEST(TestReset);
    CPPUNIT_TEST(TestSetGetState);
    CPPUNIT_TEST(TestSetGetLowerBound);
    CPPUNIT_TEST(TestSetGetUpperBound);
    CPPUNIT_TEST(TestSetGetMin);
    CPPUNIT_TEST(TestSetGetMax);
    CPPUNIT_TEST(TestSetGetStep);
    CPPUNIT_TEST(TestSetGetArray);
    CPPUNIT_TEST(TestGetDOFNumber);
    CPPUNIT_TEST(TestGetConstrainAxis);
    CPPUNIT_TEST(TestGetConstrainPlane);
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestConstructorDestructor();
    void TestDeepCopy();
    void TestReset();
    void TestSetGetState();
    void TestSetGetLowerBound();
    void TestSetGetUpperBound();
    void TestSetGetMin();
    void TestSetGetMax();
    void TestSetGetStep();
    void TestSetGetArray();
    void TestGetDOFNumber();
    void TestGetConstrainAxis();
    void TestGetConstrainPlane();
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
  runner.addTest( vtkMAFDOFMatrixTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
