/*=========================================================================

 Program: MAF2
 Module: vtkMAFDataArrayDescriptorTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFDataArrayDescriptorTest_H__
#define __CPP_UNIT_vtkMAFDataArrayDescriptorTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

//-----------------------------------------------------
// forward references:
//-----------------------------------------------------

class vtkImageUnPacker;

class vtkMAFDataArrayDescriptorTest : public CPPUNIT_NS::TestFixture
{
public:
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( vtkMAFDataArrayDescriptorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetName );
  CPPUNIT_TEST( TestSetDataType );
  CPPUNIT_TEST( TestGetDataTypeSize );
  CPPUNIT_TEST( TestSetNumberOfComponents );
  CPPUNIT_TEST( TestSetNumberOfTuples );
  CPPUNIT_TEST( TestGetSize );
  CPPUNIT_TEST( TestGetDataTypeMin );
  CPPUNIT_TEST( TestGetDataTypeMax );
  CPPUNIT_TEST( TestGetDataTypeRange );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestSetName();
  void TestSetDataType();
  void TestGetDataTypeSize();
  void TestSetNumberOfComponents();
  void TestSetNumberOfTuples();
  void TestGetSize();
  void TestGetActualMemorySize();
  void TestGetDataTypeMin();
  void TestGetDataTypeMax();
  void TestGetDataTypeRange();

};

#endif