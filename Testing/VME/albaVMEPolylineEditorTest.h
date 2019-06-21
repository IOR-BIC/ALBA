/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineEditorTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEPolylineEditorTest_H__
#define __CPP_UNIT_albaVMEPolylineEditorTest_H__

#include "albaTest.h"

class vtkPolyData;

class albaVMEPolylineEditorTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaVMEPolylineEditorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetData );
  CPPUNIT_TEST( TestSetGetVisualPipe );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestSetData();
  void TestSetGetVisualPipe();
  void CreateExamplePolydata(vtkPolyData *polydata);
};

#endif