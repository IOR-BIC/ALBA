/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkFEMCutterTest.h,v $
Language:  C++
Date:      $Date: 2007-06-21 14:03:48 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_VTKFEMCUTTERTEST_H__
#define __CPP_UNIT_VTKFEMCUTTERTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>


//------------------------------------------------------------------------------
// Test class for vtkFEMCutter
//------------------------------------------------------------------------------
class vtkFEMCutterTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkFEMCutterTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestGetOutputHex8 );
    CPPUNIT_TEST( TestGetOutputHex8_FaceInPlane );
    CPPUNIT_TEST( TestGetOutputHex8_EdgeInPlane );
    CPPUNIT_TEST( TestGetOutputHex8_CornerInPlane );
    CPPUNIT_TEST( TestGetOutputTet4 );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestGetOutputHex8();
    void TestGetOutputHex8_FaceInPlane();
    void TestGetOutputHex8_EdgeInPlane();
    void TestGetOutputHex8_CornerInPlane();
    void TestGetOutputTet4();

    // return true if v = (1-lambda)*v0 + lambda*v1
    bool ColinearVectors(double *v, double *v0, double *v1, double lambda, double ftol) ;

    // return true if point is in the plane
    bool PointInPlane(double *v, double *origin, double *norm, double ftol) ;

    // are two points equal
    bool PointsEqual(double *v, double *v0, double ftol) ;
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
  runner.addTest( vtkFEMCutterTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
