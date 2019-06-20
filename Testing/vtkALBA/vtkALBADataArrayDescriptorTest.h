/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBADataArrayDescriptorTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBADataArrayDescriptorTest_H__
#define __CPP_UNIT_vtkALBADataArrayDescriptorTest_H__

#include "albaTest.h"

//-----------------------------------------------------
// forward references:
//-----------------------------------------------------

class vtkImageUnPacker;

class vtkALBADataArrayDescriptorTest : public albaTest
{
public:
  CPPUNIT_TEST_SUITE( vtkALBADataArrayDescriptorTest );
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