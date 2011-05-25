/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFRulerActor2DTest.h,v $
Language:  C++
Date:      $Date: 2011-05-25 11:53:13 $
Version:   $Revision: 1.1.2.2 $
Authors:   Alberto Losi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMAFRulerActor2DTest_H__
#define __CPP_UNIT_vtkMAFRulerActor2DTest_H__

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

/**
  Class Name: vtkMAFGridActorTest.
  Test for vtkMAFGridActor.
*/
class vtkMAFRulerActor2DTest : public CPPUNIT_NS::TestFixture
{
  public:
    /** Test IDs*/
    enum ID_TEST_LIST
    {
      ID_SET_COLOR_TEST = 0,
      ID_SET_LABEL_SCALE_VISIBILITY_TEST,
      ID_SET_LABEL_AXES_VISIBILITY_TEST,
      ID_SET_AXES_VISIBILITY_TEST,
      ID_SET_TICK_VISIBILITY_TEST,
      ID_SET_LEGEND_TEST,
      ID_SET_GET_SCALE_FACTOR_TEST,
      ID_USE_GLOBAL_AXES_TEST,
      ID_SET_INVERSE_TICKS_TEST,
      ID_SET_ATTACH_POSITION_TEST,
      ID_CHANGE_RULER_MARGINS_AND_LENGHTS_TEST,
      ID_SET_TEXT_TEST,
    };

    /** CPPUNIT fixture: executed before each test.*/
    void setUp();
    /* CPPUNIT fixture: executed after each test.*/
    void tearDown();
    /** Test suite begin. */
    CPPUNIT_TEST_SUITE( vtkMAFRulerActor2DTest );
    /** Instance creation and deletion test. */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test SetColor. */
    CPPUNIT_TEST( TestSetColor );
    /** Test SetLabelScaleVisibility. */
    CPPUNIT_TEST( TestSetLabelScaleVisibility );
    /** Test SetLabelAxesVisibility. */
    CPPUNIT_TEST( TestSetLabelAxesVisibility );
    /** Test SetAxesVisibility. */
    CPPUNIT_TEST( TestSetAxesVisibility );
    /** Test SetTickVisibility. */
    CPPUNIT_TEST( TestSetTickVisibility );
    /** Test SetLegend. */
    CPPUNIT_TEST( TestSetLegend );
    /**Test SetScaleFactor and GetScaleFactor. */
    CPPUNIT_TEST( TestSetGetScaleFactor );
    /** Test UseGlobalAxes. */
    CPPUNIT_TEST( TestUseGlobalAxes );
    /** Test SetInverseTicks. */
    CPPUNIT_TEST( TestSetInverseTicks );
    /**Test SetAttachPositionFlag. */
    /** Test SetAttachPosition. */
    CPPUNIT_TEST( TestSetAttachPosition );
    /** Test ChangeRulerMarginsAndLengths. */
    CPPUNIT_TEST( TestChangeRulerMarginsAndLengths );
    /** Test SetText. */
    CPPUNIT_TEST( TestSetText );
    /** Test PrintSelf. */
    CPPUNIT_TEST( TestPrintSelf );
    /** Test suite end. */
    CPPUNIT_TEST_SUITE_END();

  //protected:
public:
  /** Test the object creation and deletion.*/
  void TestDynamicAllocation();
  void TestSetColor();
  void TestSetLabelScaleVisibility();
  void TestSetLabelAxesVisibility();
  void TestSetAxesVisibility();
  void TestSetTickVisibility();
  void TestSetLegend();
  void TestSetGetScaleFactor();
  void TestUseGlobalAxes();
  void TestSetInverseTicks();
  void TestSetAttachPosition();
  void TestChangeRulerMarginsAndLengths();
  void TestSetText();
  /** Call PrintSelf in order to print information in console output.*/
  void TestPrintSelf();
  /** Render data in vtk render window.*/
  void RenderData(vtkActor2D *actor );
  /** After saving images from render window, compare them in order to find difference between test sessions. */
  void CompareImages(vtkRenderWindow * renwin);
  
  static std::string ConvertInt(int number);

  int m_TestNumber;
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
  runner.addTest( vtkMAFRulerActor2DTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
  
}

#endif
