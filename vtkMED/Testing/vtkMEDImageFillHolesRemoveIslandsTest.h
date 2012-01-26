/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDImageFillHolesRemoveIslandsTest.h,v $
Language:  C++
Date:      $Date: 2012-01-26 13:45:58 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi

================================================================================
Copyright (c) 2007 Cineca, UK (www.cineca.it)
All rights reserved.
===============================================================================*/

#ifndef __CPP_UNIT_vtkMEDImageFillHolesRemoveIslandsTest_H__
#define __CPP_UNIT_vtkMEDImageFillHolesRemoveIslandsTest_H__

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
class vtkActor;

//------------------------------------------------------------------------------
// Test class for vtkMEDImageFillHolesRemoveIslands
//------------------------------------------------------------------------------
class vtkMEDImageFillHolesRemoveIslandsTest : public CPPUNIT_NS::TestFixture
{
  public:

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMEDImageFillHolesRemoveIslandsTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestFillHoles );
    CPPUNIT_TEST( TestRemoveIslands );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestFillHoles();
    void TestRemoveIslands();
    
    //accessories
    void RenderData(vtkActor *actor );
    void CompareImages(vtkRenderWindow * renwin);
    void TestAlgorithm();

    int m_Algorithm;
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
  runner.addTest( vtkMEDImageFillHolesRemoveIslandsTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
