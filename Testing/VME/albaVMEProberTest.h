/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEProberTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEProberTest_H__
#define __CPP_UNIT_albaVMEProberTest_H__

#include "albaTest.h"

/** Test for albaVMEProber */
class albaVMEProberTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaVMEProberTest );
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
