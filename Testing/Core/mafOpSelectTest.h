/*=========================================================================

 Program: MAF2
 Module: mafOpSelectTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpSelectTest_H__
#define __CPP_UNIT_mafOpSelectTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

//forward reference
class mafOpSelect;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOpSelectTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpSelectTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestSetInput);
  CPPUNIT_TEST(TestSetNewSel);
  CPPUNIT_TEST(TestCopy);
  CPPUNIT_TEST(TestOpDo);
  CPPUNIT_TEST(TestOpUndo);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAccept();
  void TestSetInput();
  void TestSetNewSel();
  void TestCopy();
  void TestOpDo();
  void TestOpUndo();

  mafOpSelect *m_OpSelect;
};

#endif
