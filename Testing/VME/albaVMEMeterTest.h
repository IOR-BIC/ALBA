/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeterTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEMeterTest_H__
#define __CPP_UNIT_albaVMEMeterTest_H__

#include "albaTest.h"

/** Test for albaVMEProber */
class albaVMEMeterTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaVMEMeterTest );
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

#endif
