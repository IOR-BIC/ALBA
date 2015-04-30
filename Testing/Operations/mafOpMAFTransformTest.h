/*=========================================================================

 Program: MAF2
 Module: mafOpMAFTransformTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpMAFTransformTest_H__
#define __CPP_UNIT_mafOpMAFTransformTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafOpMAFTransform; Use this suite to trace memory problems */
class mafOpMAFTransformTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpMAFTransformTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOnEventGizmoTranslate );
  CPPUNIT_TEST( TestOnEventGizmoRotate );
  CPPUNIT_TEST( TestOnEventGizmoScale );
  CPPUNIT_TEST( TestOnEventGuiTransform );
  CPPUNIT_TEST( TestOnEventGuiSaveRestorePose );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST( TestOpUndo );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAccept();
  void TestOnEventGizmoTranslate();
  void TestOnEventGizmoRotate();
  void TestOnEventGizmoScale();
  void TestOnEventGuiTransform();
  void TestReset();
  void TestOnEventGuiSaveRestorePose();
  void TestOpUndo();
};

#endif
