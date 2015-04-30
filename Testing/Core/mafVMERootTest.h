/*=========================================================================

 Program: MAF2
 Module: mafVMERootTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMERootTest_H__
#define __CPP_UNIT_mafVMERootTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafVMERoot; Use this suite to trace memory problems */
class mafVMERootTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafVMERootTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestGetLocalTimeStamps);
  CPPUNIT_TEST(TestSetMaxItemId);
  CPPUNIT_TEST(TestResetMaxItemId);
  CPPUNIT_TEST(TestGetNextItemId);
  CPPUNIT_TEST(TestCleanTree);
  CPPUNIT_TEST(TestCanReparentTo);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestEquals();
  void TestDeepCopy();
  void TestGetLocalTimeStamps();
  void TestSetMaxItemId();
  void TestResetMaxItemId();
  void TestGetNextItemId();
  void TestCleanTree();
  void TestCanReparentTo();
  bool result;
};


#endif

