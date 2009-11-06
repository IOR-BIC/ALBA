/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkPackedImageTest.h,v $
Language:  C++
Date:      $Date: 2009-11-06 16:04:05 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkPackedImagetest_H__
#define __CPP_UNIT_vtkPackedImagetest_H__

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


//------------------------------------------------------------------------------
// Test class for vtkPackedImage
//------------------------------------------------------------------------------
class vtkPackedImageTest : public CPPUNIT_NS::TestFixture
{
  public:
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkPackedImageTest );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestPrintSelf );
    CPPUNIT_TEST( TestSetGetPackType );
    CPPUNIT_TEST( TestSetGetImageSize );
    CPPUNIT_TEST( TestImportImage_GetImageImporter );
    CPPUNIT_TEST_SUITE_END();

  protected:
    void TestDynamicAllocation();
    void TestPrintSelf();
    void TestSetGetPackType();
    void TestSetGetImageSize();
    void TestImportImage_GetImageImporter();
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
  runner.addTest( vtkPackedImageTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
