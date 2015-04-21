/*=========================================================================

 Program: MAF2
 Module: RingDataTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_RingDataTest_H__
#define __CPP_UNIT_RingDataTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


class RingDataTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( RingDataTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAllocateVertices );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestAllocateVertices();

};

#endif