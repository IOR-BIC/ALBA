/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFTextOrientatorTest.h,v $
Language:  C++
Date:      $Date: 2007-11-26 12:53:07 $
Version:   $Revision: 1.1 $
Authors:   Daniele Giunchi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMAFTextOrientatortest_H__
#define __CPP_UNIT_vtkMAFTextOrientatortest_H__

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

//------------------------------------------------------------------------------
// Test class for vtkMAFTextOrientator
//------------------------------------------------------------------------------
class vtkMAFTextOrientatorTest : public CPPUNIT_NS::TestFixture
{
  public:
    enum ID_TEST_LIST
    {
      ID_TEXT_TEST = 0,
      ID_SINGLE_VISIBILITY_TEST,
      ID_TEXT_COLOR_TEST,
      ID_BACKGROUND_COLOR_TEST,
      ID_BACKGROUND_VISIBILITY_TEST,
      ID_SCALE_TEST
    };

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFTextOrientatorTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestText );
    CPPUNIT_TEST( TestSingleActorVisibility );
    CPPUNIT_TEST( TestTextColor );
    CPPUNIT_TEST( TestBackgroundColor );
    CPPUNIT_TEST( TestBackgroundVisibility );
    CPPUNIT_TEST( TestScale );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestText();
    void TestSingleActorVisibility();
    void TestTextColor();
    void TestBackgroundColor();
    void TestBackgroundVisibility();
    void TestScale();

    //accessories
    void SetText(vtkMAFTextOrientator *actor);
    void RenderData(vtkActor2D *actor );

    void CompareImages(vtkRenderWindow * renwin);
    
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
  runner.addTest( vtkMAFTextOrientatorTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
