/*=========================================================================

 Program: MAF2
 Module: mafOpSegmentationRegionGrowingConnectedThresholdTest
 Authors: Matteo Giacomoni, Di Cosmo Grazia
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpSegmentationRegionGrowingConnectedThresholdTest_H__
#define __CPP_UNIT_mafOpSegmentationRegionGrowingConnectedThresholdTest_H__

#include "mafTest.h"

class vtkImageData;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOpSegmentationRegionGrowingConnectedThresholdTest : public mafTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpSegmentationRegionGrowingConnectedThresholdTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestAlgorithm );
  CPPUNIT_TEST( TestAlgorithmRG );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAccept();
  void TestAlgorithm();
  void TestAlgorithmRG();

  void CompareImageData(vtkImageData *imITK,vtkImageData *imOP);

  bool m_Result;
};

#endif
