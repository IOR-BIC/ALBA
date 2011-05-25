/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFTextActorMeterTest.h,v $
Language:  C++
Date:      $Date: 2011-05-25 11:53:13 $
Version:   $Revision: 1.1.2.2 $
Authors:   Daniele Giunchi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMAFTextActorMetertest_H__
#define __CPP_UNIT_vtkMAFTextActorMetertest_H__

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
// Test class for vtkMAFTextActorMeter
//------------------------------------------------------------------------------
class vtkMAFTextActorMeterTest : public CPPUNIT_NS::TestFixture
{
  public:
    enum ID_TEST_LIST
    {
      ID_TEXT_TEST_POS1 = 0,
      ID_TEXT_TEST_POS2,
      ID_TEXT_COLOR_TEST,
    };

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFTextActorMeterTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestTextPos1 );
    CPPUNIT_TEST( TestTextPos2 );
    CPPUNIT_TEST( TestTextColor );
    CPPUNIT_TEST( TestPrintSelf );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestTextPos1();
    void TestTextPos2();
    void TestTextColor();
    void TestPrintSelf();
    /*void TestBackgroundColor();
    void TestBackgroundVisibility();
    void TestScale();*/

    //accessories
    void SetText(vtkMAFTextActorMeter *actor, const char *text, double position[3]);
    void RenderData(vtkActor2D *actor );

    void CompareImages(vtkRenderWindow * renwin);

    static std::string ConvertInt(int number);
    static std::string ConvertDouble(double number);
    
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
  runner.addTest( vtkMAFTextActorMeterTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
