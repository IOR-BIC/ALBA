/*=========================================================================

 Program: MAF2
 Module: vtkMAFTransferFunction2DTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFTransferFunction2DTest_H__
#define __CPP_UNIT_vtkMAFTransferFunction2DTest_H__

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
// Test class for vtkMAFTransferFunction2DTest
//------------------------------------------------------------------------------
class vtkMAFTransferFunction2DTest : public CPPUNIT_NS::TestFixture
{
  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFTransferFunction2DTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TetsInitialize );
    CPPUNIT_TEST( TestDeepCopy );
    CPPUNIT_TEST( TestShallowCopy );    
    CPPUNIT_TEST( TestGetDataObjectType );
    CPPUNIT_TEST( TestGetNumberOfWidgets );
    CPPUNIT_TEST( TestAddRemoveWidget );
    CPPUNIT_TEST( TestSetGetWidget );
    CPPUNIT_TEST( TestSetGetWidgetName );
    CPPUNIT_TEST( TestSetGetWidgetVisibility );
    CPPUNIT_TEST( TestSetGetWidgetOpacity );
    CPPUNIT_TEST( TestSetGetWidgetColor );
    CPPUNIT_TEST( TestSetGetWidgetDiffuse );
    CPPUNIT_TEST( TestSetGetWidgetValueRatio );
    CPPUNIT_TEST( TestSetGetWidgetValueRange );
    CPPUNIT_TEST( TestSetGetWidgetGradientRange );
    CPPUNIT_TEST( TestSetGetWidgetGradientInterpolation );
    CPPUNIT_TEST( TestCheckWidget );
    CPPUNIT_TEST( TestGetRange );
    CPPUNIT_TEST( TestGetGradientRange );
    CPPUNIT_TEST( TestGetValue );
    CPPUNIT_TEST( TestGetTable );
    CPPUNIT_TEST( TestSaveToStringLoadFromString );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestDynamicAllocation();
    void TetsInitialize();
    void TestDeepCopy();
    void TestShallowCopy();    
    void TestGetDataObjectType();
    void TestGetNumberOfWidgets();
    void TestAddRemoveWidget();
    void TestSetGetWidget();
    void TestSetGetWidgetName();
    void TestSetGetWidgetVisibility();
    void TestSetGetWidgetOpacity();
    void TestSetGetWidgetColor();
    void TestSetGetWidgetDiffuse();
    void TestSetGetWidgetValueRatio();
    void TestSetGetWidgetValueRange();
    void TestSetGetWidgetGradientRange();
    void TestSetGetWidgetGradientInterpolation();
    void TestCheckWidget();
    void TestGetRange();
    void TestGetGradientRange();
    void TestGetValue();
    void TestGetTable();
    void TestSaveToStringLoadFromString();
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
  runner.addTest( vtkMAFTransferFunction2DTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
