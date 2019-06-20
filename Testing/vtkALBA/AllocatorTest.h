/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: AllocatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_AllocatorTest_H__
#define __CPP_UNIT_AllocatorTest_H__

#include "albaTest.h"

class AllocatorTest : public albaTest
{
  CPPUNIT_TEST_SUITE( AllocatorTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestSet );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST( TestNewElements );
  CPPUNIT_TEST( TestRollBack );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestSet();
  void TestReset();
  void TestNewElements();
  void TestRollBack();

};

#endif