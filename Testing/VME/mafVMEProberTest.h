/*=========================================================================

 Program: MAF2
 Module: mafVMEProberTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEProberTest_H__
#define __CPP_UNIT_mafVMEProberTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVMEProber */
class mafVMEProberTest : public CPPUNIT_NS::TestFixture
{

public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafVMEProberTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( VolumeAcceptTest );
  CPPUNIT_TEST( OutputSurfaceAcceptTest );
  CPPUNIT_TEST( GetVisualPipeTest );
  CPPUNIT_TEST( DeepCopyTest );
  CPPUNIT_TEST( EqualsTest );
  CPPUNIT_TEST( GetSetVolumeLinkTest );
  CPPUNIT_TEST( GetSetSurfaceLinkTest );
  CPPUNIT_TEST( GetSetModeTest );
  CPPUNIT_TEST( GetSetDistanceThresholdTest );
  CPPUNIT_TEST( GetSetMaxDistanceTest );
  CPPUNIT_TEST( GetSetDistanceModeTest );
  CPPUNIT_TEST( GetSetHighDensityTest );
  CPPUNIT_TEST( GetSetLowDensityTest );
  CPPUNIT_TEST( IsDataAvailableTest );
  CPPUNIT_TEST( IsAnimatedTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void DynamicAllocationTest();
  void VolumeAcceptTest();
  void OutputSurfaceAcceptTest();
  void GetVisualPipeTest();
  void DeepCopyTest();
  void EqualsTest();
  void GetSetVolumeLinkTest();
  void GetSetSurfaceLinkTest(); 
  void GetSetModeTest(); //distance/density 
  void GetSetDistanceThresholdTest();
  void GetSetMaxDistanceTest();
  void GetSetDistanceModeTest();
  void GetSetHighDensityTest();
  void GetSetLowDensityTest();
  void IsAnimatedTest();
  void IsDataAvailableTest();
};

#endif
