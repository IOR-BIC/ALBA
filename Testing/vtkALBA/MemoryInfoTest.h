/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: MemoryInfoTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MemoryInfoTest_H__
#define __CPP_UNIT_MemoryInfoTest_H__

#include "albaTest.h"

class MemoryInfoTest : public albaTest
{
  CPPUNIT_TEST_SUITE( MemoryInfoTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestUsage1 );
  CPPUNIT_TEST( TestUsage2 );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestSet();
  void TestUsage1();
  void TestUsage2();
};

#endif