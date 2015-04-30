/*=========================================================================

 Program: MAF2
 Module: medInteractor2DIndicatorTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_medInteractor2DIndicatorTest_H__
#define __CPP_UNIT_medInteractor2DIndicatorTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mafGUIFrame;

class medInteractor2DIndicatorTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( medInteractor2DIndicatorTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestIsDisableUndoAndOkCancel );
  CPPUNIT_TEST( TestUndoMeasure );
  CPPUNIT_TEST( TestSetLabel );
  CPPUNIT_TEST( TestGetLabel );
  CPPUNIT_TEST( TestGetRegisterMeasure );
  CPPUNIT_TEST( TestSizeMeasureVector );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestIsDisableUndoAndOkCancel();
  void TestUndoMeasure();
  void TestRemoveMeter();
  void TestSetLabel();
  void TestGetLabel();
  void TestGetRegisterMeasure();
  void TestSizeMeasureVector();
};

#endif
