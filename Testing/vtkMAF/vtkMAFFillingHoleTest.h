/*=========================================================================

 Program: MAF2
 Module: vtkMAFFillingHoleTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFFillingHoleTEST_H__
#define __CPP_UNIT_vtkMAFFillingHoleTEST_H__

#include "mafTest.h"

class vtkMAFFillingHoleTest : public mafTest
{
  CPPUNIT_TEST_SUITE( vtkMAFFillingHoleTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetFillAHole );
  CPPUNIT_TEST( TestSetFillAllHole );
  CPPUNIT_TEST( TestGetLastPatch );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestSetFillAHole();
  void TestSetFillAllHole();
  void TestGetLastPatch();
};

#endif