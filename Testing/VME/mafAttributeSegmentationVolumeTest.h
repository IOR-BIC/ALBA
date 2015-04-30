/*=========================================================================

 Program: MAF2
 Module: mafAttributeSegmentationVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAttributeSegmentationVolumeTest_H__
#define __CPP_UNIT_mafAttributeSegmentationVolumeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkPolyData;

class mafAttributeSegmentationVolumeTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafAttributeSegmentationVolumeTest );
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