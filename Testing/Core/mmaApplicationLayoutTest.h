/*=========================================================================

 Program: MAF2
 Module: mmaApplicationLayoutTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmaApplicationLayoutTest_H__
#define __CPP_UNIT_mmaApplicationLayoutTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafMatrix; Use this suite to trace memory problems */
class mmaApplicationLayoutTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mmaApplicationLayoutTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAddView);
  CPPUNIT_TEST(TestRemoveView);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestClearLayout);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAddView();
  void TestRemoveView();
  void TestEquals();
  void TestDeepCopy();
  void TestClearLayout();

  wxFrame *m_Toplevel;
  bool result;
};

#endif

