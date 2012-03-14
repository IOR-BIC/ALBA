/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEMeterTest.h,v $
Language:  C++
Date:      $Date: 2010-03-10 11:25:08 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_MAFVMELANDMARKCLOUDTEST_H__
#define __CPP_UNIT_MAFVMELANDMARKCLOUDTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVMEProber */
class mafVMEMeterTest : public CPPUNIT_NS::TestFixture
{

public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafVMEMeterTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( VolumeAcceptTest );
  CPPUNIT_TEST( GetVisualPipeTest );
  CPPUNIT_TEST( DeepCopyTest );
  CPPUNIT_TEST( EqualsTest );
  CPPUNIT_TEST( GetPolylineOutputTest );
  CPPUNIT_TEST( GetSetMeterModeTest );
  CPPUNIT_TEST( GetSetDistanceRangeTest );
  CPPUNIT_TEST( GetSetMeterColorModeTest );
  CPPUNIT_TEST( GetSetMeterMeasureType );
  CPPUNIT_TEST( GetSetMeterRepresentationTest );
  CPPUNIT_TEST( GetSetMeterCappingTest );
  CPPUNIT_TEST( GetSetGenerateEventTest );
  CPPUNIT_TEST( GetSetInitMeasureTest );
  CPPUNIT_TEST( GetSetMeterRadiusTest );
  CPPUNIT_TEST( GetSetDeltaPercentTest );
  CPPUNIT_TEST( GetDistanceTest );
  CPPUNIT_TEST( GetAngleTest );
  CPPUNIT_TEST( GetMeterAttributesTest );
  CPPUNIT_TEST( GetSetMeterLinkTest );
  CPPUNIT_TEST( GetStartPointCoordinateTest );
  CPPUNIT_TEST( GetEndPointCoordinateTest );
  CPPUNIT_TEST( GetEndPoint2CoordinateTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void DynamicAllocationTest();
  void VMEAcceptTest();
  void VolumeAcceptTest();
  void GetVisualPipeTest();
  void DeepCopyTest();
  void EqualsTest();
  void IsAnimatedTest();
  void GetPolylineOutputTest();
  void GetSetGenerateEventTest();
  void GetSetMeterModeTest(); // point distance, line distance, line angle
  void GetSetDistanceRangeTest();
  void GetSetMeterColorModeTest(); //one color, range color
  void GetSetMeterMeasureType(); //absolute, relative
  void GetSetMeterRepresentationTest(); //line, tube
  void GetSetMeterCappingTest();
  void GetSetInitMeasureTest();
  void GetSetMeterRadiusTest();
  void GetSetDeltaPercentTest();
  void GetDistanceTest();
  void GetAngleTest();
  void GetMeterAttributesTest ();
  void GetSetMeterLinkTest();
  void GetStartPointCoordinateTest();
  void GetEndPointCoordinateTest();
  void GetEndPoint2CoordinateTest();

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
  runner.addTest( mafVMEMeterTest::suite());

  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
