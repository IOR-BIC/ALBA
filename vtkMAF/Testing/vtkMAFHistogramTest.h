/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFHistogramTest.h,v $
Language:  C++
Date:      $Date: 2011-05-25 11:53:13 $
Version:   $Revision: 1.1.2.2 $
Authors:   Daniele Giunchi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMAFHistogramtest_H__
#define __CPP_UNIT_vtkMAFHistogramtest_H__

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
  Class Name: vtkMAFHistogramTest.
  Test for vtkMAFHistogram.
*/
class vtkMAFHistogramTest : public CPPUNIT_NS::TestFixture
{
  public:
    /** Test IDs*/
    enum ID_TEST_LIST
    {
      ID_HISTOGRAM_POINT_REPRESENTATION_TEST = 0,
      ID_HISTOGRAM_LINE_REPRESENTATION_TEST,
      ID_HISTOGRAM_BAR_REPRESENTATION_TEST,
      ID_HISTOGRAM_LOGARITMIC_PROPERTIES_TEST,
      ID_HISTOGRAM_SCALE_COLOR_PROPERTIES,
      ID_HISTOGRAM_GET_ATTRIBUTES_AND_UPDATE_LINES,
    };

    /** CPPUNIT fixture: executed before each test.*/
    void setUp();

    /* CPPUNIT fixture: executed after each test.*/
    void tearDown();

    /** Test suite begin. */
    CPPUNIT_TEST_SUITE( vtkMAFHistogramTest );
    /** Instance creation and deletion test. */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test Point Representation */
    CPPUNIT_TEST( TestHistogramPointRepresentation );
    /** Test Line Representation */
    CPPUNIT_TEST( TestHistogramLineRepresentation );
    /** Test Bar Representation */
    CPPUNIT_TEST( TestHistogramBarRepresentation );
    /** Test Histogram Logaritmic Scale Properties*/
    CPPUNIT_TEST( TestHistogramLogaritmicProperties );
    /** Test Histogram Scale Factor and Color Properties*/
    CPPUNIT_TEST( TestHistogramScaleFactorColorProperties );
    /** Test Histogram Get Attributes*/
    CPPUNIT_TEST( TestHistogramGetAttributesAndUpdateLines );
    /** Test PrintSelf. */
    CPPUNIT_TEST( TestPrintSelf );
    /** Test suite end. */
    CPPUNIT_TEST_SUITE_END();

  protected:

    /** Test the object creation and deletion.*/
    void TestDynamicAllocation();
    /** Test Histogram with Point Representation*/
    void TestHistogramPointRepresentation();
    /** Test Histogram with Line Representation*/
    void TestHistogramLineRepresentation();
    /** Test Histogram with Bar Representation*/
    void TestHistogramBarRepresentation();
    /** Test Histogram Logaritmic Scale Properties*/
    void TestHistogramLogaritmicProperties();
    /** Test Histogram Scale Factor and Color Properties*/
    void TestHistogramScaleFactorColorProperties();
    /** Test Histogram Get Attributes and Update Lines*/
    void TestHistogramGetAttributesAndUpdateLines();
    /** Call PrintSelf in order to print information in console output.*/
    void TestPrintSelf();

    /** Render data in vtk render window.*/
    void RenderData(vtkActor2D *actor );
    /** After saving images from render window, compare them in order to find difference between test sessions. */
    void CompareImages(vtkRenderWindow * renwin);
    /** Convert an int to std::string */
    static std::string ConvertInt(int number);

    int m_TestNumber;
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
  runner.addTest( vtkMAFHistogramTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
