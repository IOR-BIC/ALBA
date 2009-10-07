/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEMapsTest.h,v $
Language:  C++
Date:      $Date: 2009-10-07 15:17:59 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_medVMEMapsTEST_H__
#define __CPP_UNIT_medVMEMapsTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafVMESurface;
class mafVMEVolumeGray;
class medVMEMaps;

class medVMEMapsTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( medVMEMapsTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestPrint );
  CPPUNIT_TEST( TestGetLocalTimeStamps );
  CPPUNIT_TEST( TestGetDensityDistance );
  CPPUNIT_TEST( TestGetFirstThreshold );
  CPPUNIT_TEST( TestGetSecondThreshold );
  CPPUNIT_TEST( TestGetMaxDistance );
  CPPUNIT_TEST( TestGetVisualPipe );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestPrint();
  void TestGetLocalTimeBounds();
  void TestIsAnimated();
  void TestGetLocalTimeStamps();

  void TestGetDensityDistance();
  void TestGetFirstThreshold();
  void TestGetSecondThreshold();
  void TestGetMaxDistance();
  void TestGetMappedVMELink();
  void TestGetSourceVMELink();
  void TestGetVisualPipe();


  void CreateVMEMaps();
  void CreateVMEVolume();

  bool m_Result;

  mafVMEVolumeGray  *m_Volume;
  mafVMESurface     *m_SurfaceToMap;
  medVMEMaps        *m_Maps;

  int m_FirstThreshold;
  int m_SecondThreshold;
  int m_MaxDistance;
  int m_DensityDistance;


};


int
main( int argc, char* argv[] )
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
  runner.addTest( medVMEMapsTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
