/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeWrappingTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpComputeWrappingTest_H
#define CPP_UNIT_albaOpComputeWrappingTest_H

#include "albaTest.h"

class albaOpComputeWrappingTest : public albaTest
{
  CPPUNIT_TEST_SUITE( albaOpComputeWrappingTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestCopy );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestOpDo );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestCopy();
  void TestAccept();
  void TestOpRun();
  void TestOpDo();
};

#endif
