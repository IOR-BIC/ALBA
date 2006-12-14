/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkContourVolumeMapperTest.h,v $
Language:  C++
Date:      $Date: 2006-12-14 17:47:25 $
Version:   $Revision: 1.3 $
Authors:   Matteo Giacomoni, Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_VTKCONTOURVOLUMEMAPPERTEST_H__
#define __CPP_UNIT_VTKCONTOURVOLUMEMAPPERTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkImageData;
class vtkRectilinearGrid;

class vtkContourVolumeMapperTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkContourVolumeMapperTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestImageData );
    CPPUNIT_TEST( TestRectilinearGrid );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestImageData();
    void TestRectilinearGrid();

		void CreateDataID();
		void CreateDataRG();
    vtkImageData *m_ImageData;
    vtkRectilinearGrid *m_RGData;

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
  runner.addTest( vtkContourVolumeMapperTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
