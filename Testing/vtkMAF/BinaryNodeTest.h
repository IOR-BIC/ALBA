/*=========================================================================

 Program: MAF2
 Module: BinaryNodeTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_BinaryNodeTest_H__
#define __CPP_UNIT_BinaryNodeTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class BinaryNodeTest : public CPPUNIT_NS::TestFixture
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