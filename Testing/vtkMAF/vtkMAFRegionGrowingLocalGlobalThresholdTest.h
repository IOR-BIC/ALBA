/*=========================================================================

 Program: MAF2
 Module: vtkMAFRegionGrowingLocalGlobalThresholdTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFRegionGrowingLocalGlobalThresholdTest_H__
#define __CPP_UNIT_vtkMAFRegionGrowingLocalGlobalThresholdTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkImageData;
class vtkMAFRegionGrowingLocalGlobalThreshold;

//-----------------------------------------------------
// forward references:
//-----------------------------------------------------

class vtkPolyData;

class vtkMAFRegionGrowingLocalGlobalThresholdTest : public CPPUNIT_NS::TestFixture
{
public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( vtkMAFRegionGrowingLocalGlobalThresholdTest );
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
  vtkMAFRegionGrowingLocalGlobalThreshold *m_Filter;

};

#endif