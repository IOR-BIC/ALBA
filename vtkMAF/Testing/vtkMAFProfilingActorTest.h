/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFProfilingActorTest.h,v $
Language:  C++
Date:      $Date: 2009-09-09 08:16:31 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMAFProfilingActorTest_H__
#define __CPP_UNIT_vtkMAFProfilingActorTest_H__

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
class vtkRenderWindow;
class vtkImageData;
class vtkMAFProfilingActor;

/**
  Class Name: vtkMAFProfilingActorTest.
  Test for vtkMAFProfilingActor.
*/
class vtkMAFProfilingActorTest : public CPPUNIT_NS::TestFixture
{
  public:
    /** CPPUNIT fixture: executed before each test.*/
    void setUp();

    /* CPPUNIT fixture: executed after each test.*/
    void tearDown();

    /** Test suite begin. */
    CPPUNIT_TEST_SUITE( vtkMAFProfilingActorTest );
    /** Instance creation and deletion test. */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test RenderOverlay. */
    CPPUNIT_TEST( TestRenderOverlay );
    /** Test RenderOpaqueGeometry. */
    CPPUNIT_TEST( TestRenderOpaqueGeometry );
    /** Test RenderTranslucentGeometry. */
    CPPUNIT_TEST( TestRenderTranslucentGeometry );
    /** Test PrintSelf. */
    CPPUNIT_TEST( TestPrintSelf );
    CPPUNIT_TEST_SUITE_END();

  protected:

    /** Test the object creation and deletion.*/
    void TestDynamicAllocation();

    /** Test RenderOverlay.*/
    void TestRenderOverlay();
    /** Test RenderOpaqueGeometry.*/
    void TestRenderOpaqueGeometry();
    /** Test RenderTranslucentGeometry.*/
    void TestRenderTranslucentGeometry();
    /** Test PrintSelf.*/
    void TestPrintSelf();

    /** Render data in vtk render window.*/
    void PrepareToRender(vtkRenderer *renderer, vtkRenderWindow *renderWindow);
    /** After saving images from render window, compare them in order to find difference between test sessions. */
    void CompareImages(vtkRenderWindow * renwin);

    int m_TestNumber;
};

int main( int argc, char* argv[] )
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
  runner.addTest( vtkMAFProfilingActorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
