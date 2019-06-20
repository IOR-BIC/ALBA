/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSmoothSurfaceTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpSmoothSurfaceTest_H
#define CPP_UNIT_albaOpSmoothSurfaceTest_H

#include "albaTest.h"

class albaOpSmoothSurfaceTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpSmoothSurfaceTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOpRun();
};

#endif
