/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSmoothSurfaceCellsTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpSmoothSurfaceCellsTest_H
#define CPP_UNIT_albaOpSmoothSurfaceCellsTest_H

#include "albaTest.h"

class albaOpSmoothSurfaceCellsTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpSmoothSurfaceCellsTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestSetDiameter );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestSetSeed );
  CPPUNIT_TEST( TestMarkCells );
  CPPUNIT_TEST( TestSmoothCells );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestAccept();
  void TestSetDiameter();
  void TestOpRun();
  void TestSetSeed();
  void TestMarkCells();
  void TestSmoothCells();
};

#endif
