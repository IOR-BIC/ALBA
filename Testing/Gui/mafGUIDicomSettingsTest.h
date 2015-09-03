/*=========================================================================

 Program: MAF2
 Module: mafGUIDicomSettingsTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUIDicomSettingsTest_H__
#define __CPP_UNIT_mafGUIDicomSettingsTest_H__

#include "mafTest.h"

class mafGUIDicomSettingsTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUIDicomSettingsTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetDictionary );
  CPPUNIT_TEST( TestAutoCropPosition );
  CPPUNIT_TEST( TestEnableNumberOfTime );
  CPPUNIT_TEST( TestEnableNumberOfSlice );
  CPPUNIT_TEST( TestGetBuildStep );
  CPPUNIT_TEST( TestEnableChangeSide );
  CPPUNIT_TEST( TestEnableDiscardPosition );
  CPPUNIT_TEST( TestEnableResampleVolume );
  CPPUNIT_TEST( TestEnableRescaleTo16Bit );
  CPPUNIT_TEST( TestEnableVisualizationPosition );
  CPPUNIT_TEST( TestEnableZCrop );
  CPPUNIT_TEST( TestEnableScalarTolerance );
  CPPUNIT_TEST( TestGetScalarTolerance );
  CPPUNIT_TEST( TestEnablePercentageTolerance );
  CPPUNIT_TEST( TestGetPercentageTolerance );
  CPPUNIT_TEST( TestAutoVMEType );
  CPPUNIT_TEST( TestGetVMEType );
  CPPUNIT_TEST( TestEnableToRead );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestGetDictionary();
  void TestAutoCropPosition();
  void TestEnableNumberOfTime();
  void TestEnableNumberOfSlice();
  void TestGetBuildStep();
  void TestEnableChangeSide();
  void TestEnableDiscardPosition();
  void TestEnableResampleVolume();
  void TestEnableRescaleTo16Bit();
  void TestEnableVisualizationPosition();
  void TestEnableZCrop();
  void TestEnableScalarTolerance();
  void TestGetScalarTolerance();
  void TestEnablePercentageTolerance();
  void TestGetPercentageTolerance();
  void TestAutoVMEType();
  void TestGetVMEType();
  void TestEnableToRead();

  bool m_Result;
};

#endif
