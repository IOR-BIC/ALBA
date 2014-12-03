/*=========================================================================

 Program: MAF2
 Module: mafOpMML3ParameterViewTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpMML3ParameterViewTest_H__
#define __CPP_UNIT_mafOpMML3ParameterViewTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

class mafOpMML3ParameterViewTest : public CPPUNIT_NS::TestFixture
{
public:

  void setUp();
  void tearDown();

  CPPUNIT_TEST_SUITE( mafOpMML3ParameterViewTest );
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
  runner.addTest( mafOpMML3ParameterViewTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
