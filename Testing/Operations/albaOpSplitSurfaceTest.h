/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSplitSurfaceTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpSplitSurfaceTest_H
#define CPP_UNIT_albaOpSplitSurfaceTest_H

#include "albaTest.h"

class albaOpSplitSurfaceTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpSplitSurfaceTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestClip );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestClip();
};

#endif
