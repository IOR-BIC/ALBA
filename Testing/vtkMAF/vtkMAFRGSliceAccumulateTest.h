/*=========================================================================

 Program: MAF2
 Module: vtkMAFRGSliceAccumulateTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFRGSliceAccumulateTest_H__
#define __CPP_UNIT_vtkMAFRGSliceAccumulateTest_H__

#include "mafTest.h"

class vtkMAFRGSliceAccumulateTest : public mafTest
{
public:

  CPPUNIT_TEST_SUITE( vtkMAFRGSliceAccumulateTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetSlice );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestAddSlice();
  void TestSetSlice();

};

#endif
