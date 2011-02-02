/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpMML3ParameterViewTest.h,v $
Language:  C++
Date:      $Date: 2011-02-02 13:17:00 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_medOpMML3ParameterViewTest_H__
#define __CPP_UNIT_medOpMML3ParameterViewTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

class medOpMML3ParameterViewTest : public CPPUNIT_NS::TestFixture
{
public:

  void setUp();
  void tearDown();

  CPPUNIT_TEST_SUITE( medOpMML3ParameterViewTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetValue );
  CPPUNIT_TEST( TestGetNumberOfDataPoints );
  CPPUNIT_TEST( TestGetMaxY );
  CPPUNIT_TEST( TestGetMinY );
  CPPUNIT_TEST( TestSetLineActorX );
  CPPUNIT_TEST( TestGetPointsActor );
  CPPUNIT_TEST( TestGetSplineActor );
  // CPPUNIT_TEST( TestSetRangeX ); Cannot test
  // CPPUNIT_TEST( TestSetRangeY ); Already tested in TestGetMaxY, TestGetMinY
  CPPUNIT_TEST( TestRemovePoint );
  CPPUNIT_TEST( TestAddPoint );
  CPPUNIT_TEST( TestRender );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestGetValue();
  void TestGetNumberOfDataPoints();
  void TestGetMaxY();
  void TestGetMinY();
  void TestSetLineActorX();
  void TestGetPointsActor();
  void TestGetSplineActor();
  // void TestSetRangeX();
  // void TestSetRangeY();
  void TestRemovePoint();
  void TestAddPoint();
  void TestRender();

  void CompareImages(mafString test_name);

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
};

int main( int argc, char* argv[] )
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
  runner.addTest( medOpMML3ParameterViewTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
