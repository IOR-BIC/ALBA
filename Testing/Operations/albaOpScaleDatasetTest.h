/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpScaleDatasetTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpScaleDatasetTest_H
#define CPP_UNIT_albaOpScaleDatasetTest_H

#include "albaTest.h"

class albaOpScaleDatasetTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpScaleDatasetTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestOpDoVolume );
  CPPUNIT_TEST( TestOpDoSurface );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestOpDoVolume();
  void TestOpDoSurface();
  void TestReset();
  void TestAccept();
};

#endif
