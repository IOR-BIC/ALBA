/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttributeSegmentationVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAttributeSegmentationVolumeTest_H__
#define __CPP_UNIT_albaAttributeSegmentationVolumeTest_H__

#include "albaTest.h"

class vtkPolyData;

class albaAttributeSegmentationVolumeTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaAttributeSegmentationVolumeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetAutomaticSegmentationThresholdModality );
  CPPUNIT_TEST( TestSetAutomaticSegmentationGlobalThreshold );
  CPPUNIT_TEST( TestAddRange );
  CPPUNIT_TEST( TestUpdateRange );
  CPPUNIT_TEST( TestDeleteRange );
  CPPUNIT_TEST( TestRemoveAllRanges );
  CPPUNIT_TEST( TestGetNumberOfRanges );
  CPPUNIT_TEST( TestSetRegionGrowingUpperThreshold );
  CPPUNIT_TEST( TestSetRegionGrowingLowerThreshold );
  CPPUNIT_TEST( TestAddSeed );
  CPPUNIT_TEST( TestDeleteSeed );
  CPPUNIT_TEST( TestGetNumberOfSeeds );
  CPPUNIT_TEST( TestRemoveAllSeeds );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestSetAutomaticSegmentationThresholdModality();
  void TestSetAutomaticSegmentationGlobalThreshold();
  void TestAddRange();
  void TestUpdateRange();
  void TestDeleteRange();
  void TestRemoveAllRanges();
  void TestGetNumberOfRanges();
  void TestSetRegionGrowingUpperThreshold();
  void TestSetRegionGrowingLowerThreshold();
  void TestAddSeed();
  void TestDeleteSeed();
  void TestGetNumberOfSeeds();
  void TestRemoveAllSeeds();
};

#endif