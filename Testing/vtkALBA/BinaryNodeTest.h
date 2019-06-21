/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: BinaryNodeTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_BinaryNodeTest_H__
#define __CPP_UNIT_BinaryNodeTest_H__

#include "albaTest.h"

class BinaryNodeTest : public albaTest
{
  CPPUNIT_TEST_SUITE( BinaryNodeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestCenterCount );
  CPPUNIT_TEST( TestCumulativeCenterCount );
  CPPUNIT_TEST( TestIndex );
  CPPUNIT_TEST( TestCornerIndex );
  CPPUNIT_TEST( TestCornerIndexPosition );
  CPPUNIT_TEST( TestWidth );
  CPPUNIT_TEST( TestCenterAndWidth );
  CPPUNIT_TEST( TestDepthAndOffset );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestCenterCount();
  void TestCumulativeCenterCount();
  void TestIndex();
  void TestCornerIndex();
  void TestCornerIndexPosition();
  void TestWidth();
  void TestCenterAndWidth();
  void TestDepthAndOffset();
};

#endif