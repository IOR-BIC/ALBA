/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSegmentationRegionGrowingConnectedThresholdTest
 Authors: Matteo Giacomoni, Di Cosmo Grazia, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpSegmentationRegionGrowingConnectedThresholdTest_H__
#define __CPP_UNIT_albaOpSegmentationRegionGrowingConnectedThresholdTest_H__

#include "albaTest.h"

class vtkImageData;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpSegmentationRegionGrowingConnectedThresholdTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpSegmentationRegionGrowingConnectedThresholdTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestAlgorithmID );
  CPPUNIT_TEST( TestAlgorithmRG );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAccept();
  void TestAlgorithmID();
  void TestAlgorithmRG();

	void TestAlgorithm(char *inputVol,char *compareVol, int seed[3], double threshold[2]);


  void CompareImageData(vtkImageData *imITK,vtkImageData *imOP);

  bool m_Result;
};

#endif
