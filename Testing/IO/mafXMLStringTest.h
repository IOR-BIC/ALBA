/*=========================================================================

 Program: MAF2
 Module: mafXMLStringTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafXMLStringTest_H__
#define __CPP_UNIT_mafXMLStringTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafXMLString;

class mafXMLStringTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( mafXMLStringTest );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAllConstructors );
  CPPUNIT_TEST( TestAppend );
  CPPUNIT_TEST( TestErase );
  CPPUNIT_TEST( TestBegin );
  CPPUNIT_TEST( TestEnd );
  CPPUNIT_TEST( TestSize );
  CPPUNIT_TEST( TestGetCStr );
  CPPUNIT_TEST( TestAllOperators );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructors();
  void TestAppend();
  void TestErase();
  void TestBegin();
  void TestEnd();
  void TestSize();
  void TestGetCStr();
  void TestAllOperators();
};

#endif 