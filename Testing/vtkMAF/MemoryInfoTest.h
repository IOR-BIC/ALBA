/*=========================================================================

 Program: MAF2
 Module: MemoryInfoTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MemoryInfoTest_H__
#define __CPP_UNIT_MemoryInfoTest_H__

#include "mafTest.h"

class MemoryInfoTest : public mafTest
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