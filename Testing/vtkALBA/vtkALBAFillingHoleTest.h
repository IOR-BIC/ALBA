/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFillingHoleTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAFillingHoleTEST_H__
#define __CPP_UNIT_vtkALBAFillingHoleTEST_H__

#include "albaTest.h"

class vtkALBAFillingHoleTest : public albaTest
{
  CPPUNIT_TEST_SUITE( vtkALBAFillingHoleTest );
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