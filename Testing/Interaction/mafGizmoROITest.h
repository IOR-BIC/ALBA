/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGizmoROITest.h,v $
Language:  C++
Date:      $Date: 2008-07-01 12:30:45 $
Version:   $Revision: 1.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoBoundingBoxTest_H__
#define __CPP_UNIT_mafGizmoBoundingBoxTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafGizmoROI.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class mafGizmoROITest : public CPPUNIT_NS::TestFixture
{
  public:
  
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafGizmoROITest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetListener);
  CPPUNIT_TEST( TestSetInput );
  CPPUNIT_TEST( TestSetGetBounds );
  CPPUNIT_TEST( TestShow );
  CPPUNIT_TEST( TestShowHandles);
  CPPUNIT_TEST( TestShowShadingPlane );
  CPPUNIT_TEST( TestShowROI );
  CPPUNIT_TEST( TestHighlight );
  CPPUNIT_TEST( TestHighlightOff );
  CPPUNIT_TEST( TestSetConstrainRefSys );
  CPPUNIT_TEST( TestEnableMinimumHandleSize );
  CPPUNIT_TEST( TestSetMinimumHandleSize );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST_SUITE_END();
  
  protected:
    
    void TestFixture();

    void TestConstructor();
    void TestSetListener();
    void TestSetInput();
    void TestShow();
    void TestShowHandles();
    void TestShowShadingPlane();
    void TestShowROI();
    void TestSetGetBounds();
    void TestHighlight();
    void TestHighlightOff();
    void TestSetConstrainRefSys();
    void TestEnableMinimumHandleSize();
    void TestSetMinimumHandleSize();
    void TestReset();

    void CreateTestData();

    void RenderGizmo( mafGizmoROI *gizmoROI );

    mafVMESurface *m_GizmoInputSurface;
    mafVMERoot *m_Root;

    // TODO: REFACTOR THIS 
    // move somewhere else... there should be a common class for testing facilities
    void RenderData(vtkDataSet *data);

    void CreateRenderStuff(); 
      
    vtkRenderer *m_Renderer;
    vtkRenderWindow *m_RenderWindow;
    vtkRenderWindowInteractor *m_RenderWindowInteractor;    
    
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
  runner.addTest( mafGizmoROITest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
