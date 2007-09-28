/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFMeshCutterTest.h,v $
Language:  C++
Date:      $Date: 2007-09-28 11:21:01 $
Version:   $Revision: 1.2 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMAFMeshCuttertest_H__
#define __CPP_UNIT_vtkMAFMeshCuttertest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>


//------------------------------------------------------------------------------
// Test class for vtkMAFMeshCutter
//------------------------------------------------------------------------------
class vtkMAFMeshCutterTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFMeshCutterTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestGetOutputHex8 );
    CPPUNIT_TEST( TestGetOutputHex8_VerticalCut1 );
    CPPUNIT_TEST( TestGetOutputHex8_VerticalCut2 );
    CPPUNIT_TEST( TestGetOutputHex8_FaceInPlane );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesEdge1 );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesEdge2 );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesCorner );
    CPPUNIT_TEST( TestGetOutputHex8_EdgeOnly );
    CPPUNIT_TEST( TestGetOutputHex8_CornerOnly );
    CPPUNIT_TEST( TestGetOutputTet4 );
    CPPUNIT_TEST( TestUpdate );
    CPPUNIT_TEST( TestWait ) ;
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestGetOutputHex8();
    void TestGetOutputHex8_VerticalCut1();
    void TestGetOutputHex8_VerticalCut2();
    void TestGetOutputHex8_FaceInPlane();
    void TestGetOutputHex8_IncludesEdge1();
    void TestGetOutputHex8_IncludesEdge2();
    void TestGetOutputHex8_IncludesCorner();
    void TestGetOutputHex8_EdgeOnly();
    void TestGetOutputHex8_CornerOnly();
    void TestGetOutputTet4();
    void TestUpdate();
    void TestWait() {Sleep(5000);} ; // empty test to generate pause

    // return true if v = (1-lambda)*v0 + lambda*v1
    bool ColinearVectors(double *v, double *v0, double *v1, double lambda, double ftol) ;

    // return true if point is in the plane
    bool PointInPlane(double *v, double *origin, double *norm, double ftol) ;

    // are two points equal
    bool PointsEqual(double *v, double *v0, double ftol) ;

    // render input and output data
    void RenderPointScalars(vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;
    void RenderCellScalars(vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;

    // test the interpolation of scalars
    void ScalarTest(vtkMAFMeshCutter *MMC, vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;
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
  runner.addTest( vtkMAFMeshCutterTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
