/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARegionGrowingLocalGlobalThresholdTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBARegionGrowingLocalGlobalThresholdTest_H__
#define __CPP_UNIT_vtkALBARegionGrowingLocalGlobalThresholdTest_H__

#include "albaTest.h"

class vtkImageData;
class vtkALBARegionGrowingLocalGlobalThreshold;

//-----------------------------------------------------
// forward references:
//-----------------------------------------------------

class vtkPolyData;

class vtkALBARegionGrowingLocalGlobalThresholdTest : public albaTest
{
public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( vtkALBARegionGrowingLocalGlobalThresholdTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestBorderCreate );
  CPPUNIT_TEST( TestComputeMeanValue );
  CPPUNIT_TEST( TestComputeStandardDeviation );
  CPPUNIT_TEST( TestExecute );
  CPPUNIT_TEST( TestSetOutputScalarType );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestBorderCreate();
  void TestComputeMeanValue();
  void TestComputeStandardDeviation();
  void TestExecute();
  void TestSetOutputScalarType();

  vtkImageData *m_Image;
  vtkALBARegionGrowingLocalGlobalThreshold *m_Filter;

};

#endif