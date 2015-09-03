/*=========================================================================

 Program: MAF2
 Module: AllocatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_AllocatorTest_H__
#define __CPP_UNIT_AllocatorTest_H__

#include "mafTest.h"

class AllocatorTest : public mafTest
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