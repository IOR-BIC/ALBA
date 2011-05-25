/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFExtendedGlyph3DTest.h,v $
Language:  C++
Date:      $Date: 2011-05-25 11:53:13 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2010 
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

class vtkPolyData;
class vtkRenderWindow;
class vtkRenderer;

class vtkMAFExtendedGlyph3DTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( vtkMAFExtendedGlyph3DTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetNumberOfSources );
  CPPUNIT_TEST( TestSetSource );
  CPPUNIT_TEST( TestSetScaling );
  CPPUNIT_TEST( TestSetScaleFactor );
  CPPUNIT_TEST( TestSetRange );
  CPPUNIT_TEST( TestSetOrient );
  CPPUNIT_TEST( TestSetClamping );
  CPPUNIT_TEST( TestSetGeneratePointIds );
  CPPUNIT_TEST( TestSetPointIdsName );
  CPPUNIT_TEST( TestSetScalarVisibility );
  CPPUNIT_TEST_SUITE_END();

protected:

  void CreatePointsSet();
  void CreateRenWindow();
  void DeleteRenWindow();
  void CompareImages();

  void TestFixture();

  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestSetNumberOfSources();
  void TestSetSource();
  void TestSetScaling();
  void TestSetScaleFactor();
  void TestSetRange();
  void TestSetOrient();
  void TestSetClamping();
  void TestSetGeneratePointIds();
  void TestSetPointIdsName();
  void TestSetScalarVisibility();

  vtkPolyData *m_Points;
  int m_TestNumber;

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenWin;

  static std::string ConvertInt(int number);

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
  runner.addTest( vtkMAFExtendedGlyph3DTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
