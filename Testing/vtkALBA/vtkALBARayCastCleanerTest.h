/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARayCastCleanerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBARayCastCleanerTEST_H__
#define __CPP_UNIT_vtkALBARayCastCleanerTEST_H__

#include "albaTest.h"

class vtkALBARayCastCleanerTest : public albaTest
{
  CPPUNIT_TEST_SUITE( vtkALBARayCastCleanerTest );
  CPPUNIT_TEST( TestFilter );
  CPPUNIT_TEST( TestFilterMR );
  CPPUNIT_TEST( TestSetterGetter );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFilter();
  void TestFilterMR();
  void TestSetterGetter();
  void TestDynamicAllocation();
};

#endif