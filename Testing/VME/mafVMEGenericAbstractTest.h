/*=========================================================================

 Program: MAF2
 Module: mafVMEGenericAbstractTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEGenericAbstractTest_H__
#define __CPP_UNIT_mafVMEGenericAbstractTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVMEGenericAbstract; Use this suite to trace memory problems */
class mafVMEGenericAbstractTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMEGenericAbstractTest );
	CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAllConstructor);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestShallowCopy);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestReparentTo);
  CPPUNIT_TEST(TestGetMatrixVector);
  CPPUNIT_TEST(TestSetMatrix);
  CPPUNIT_TEST(TestGetDataVector);
  CPPUNIT_TEST(TestGetDataTimeStamps);
  CPPUNIT_TEST(TestGetMatrixTimeStamps);
  CPPUNIT_TEST(TestGetLocalTimeStamps);
  CPPUNIT_TEST(TestGetLocalTimeBounds);
  CPPUNIT_TEST(TestIsAnimated);
  CPPUNIT_TEST(TestIsDataAvailable);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAllConstructor();
  void TestDeepCopy();
  void TestShallowCopy();
  void TestEquals();
  void TestReparentTo();
  void TestGetMatrixVector();
  void TestSetMatrix();
  void TestGetDataVector();
  void TestGetDataTimeStamps();
  void TestGetMatrixTimeStamps();
  void TestGetLocalTimeStamps();
  void TestGetLocalTimeBounds();
  void TestIsAnimated();
  void TestIsDataAvailable();

  bool result;
};

#endif