/*=========================================================================

 Program: MAF2
 Module: mafVMESegmentationVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMESegmentationVolumeTest_H__
#define __CPP_UNIT_mafVMESegmentationVolumeTest_H__

#include "mafTest.h"

class mafVMEStorage;
class mafVMESegmentationVolume;
class mafVMEVolumeGray;

class mafVMESegmentationVolumeTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafVMESegmentationVolumeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetAutomaticSegmentationThresholdModality );
  CPPUNIT_TEST( TestUpdateRange );
  CPPUNIT_TEST( TestAddRange );
  CPPUNIT_TEST( TestDeleteRange );
  CPPUNIT_TEST( TestSetAutomaticSegmentationGlobalThreshold );
  CPPUNIT_TEST( TestSetVolumeLink );
  CPPUNIT_TEST( TestAutomaticSegmentation );
  CPPUNIT_TEST( TestRefinementSegmentation );
  CPPUNIT_TEST( TestManualSegmentation );
  CPPUNIT_TEST( TestSetManualVolumeMask );
  CPPUNIT_TEST( TestSetRefinementVolumeMask );
  CPPUNIT_TEST( TestAddSeed );
  CPPUNIT_TEST( TestRemoveAllSeeds );
  CPPUNIT_TEST( TestSetRegionGrowingUpperThreshold );
  CPPUNIT_TEST( TestSetRegionGrowingLowerThreshold );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestSetAutomaticSegmentationThresholdModality();
  void TestAddRange();
  void TestUpdateRange();
  void TestDeleteRange();
  void TestSetAutomaticSegmentationGlobalThreshold();
  void TestSetVolumeLink();
  void TestAutomaticSegmentation();
  void TestRefinementSegmentation();
  void TestManualSegmentation();
  void TestSetManualVolumeMask();
  void TestSetRefinementVolumeMask();
  void TestAddSeed();
  void TestRemoveAllSeeds();
  void TestSetRegionGrowingUpperThreshold();
  void TestSetRegionGrowingLowerThreshold();

  bool m_Result;
  mafVMEStorage *m_Storage;
  mafVMEVolumeGray *m_Volume;
  mafVMEVolumeGray *m_VolumeRG;
  mafVMEVolumeGray *m_VolumeManualMask;
  mafVMEVolumeGray *m_VolumeRefinementMask;
};

#endif
