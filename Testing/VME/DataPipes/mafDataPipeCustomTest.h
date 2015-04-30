/*=========================================================================

 Program: MAF2
 Module: mafDataPipeCustomTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDataPipeCustomTest_H__
#define __CPP_UNIT_mafDataPipeCustomTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** Test for mafDataPipeCustom; Use this suite to trace memory problems */
class mafDataPipeCustomTest : public CPPUNIT_NS::TestFixture
{
public: 
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafDataPipeCustomTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);

  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestGetVTKData);
  CPPUNIT_TEST(TestGetVTKDataPipe);
  CPPUNIT_TEST(TestUpdate);
  CPPUNIT_TEST(TestUpdateBounds);
  CPPUNIT_TEST(TestSetInput);
  CPPUNIT_TEST(TestSetNthInput);
  
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestOnEvent();
  void TestGetVTKData();
  void TestGetVTKDataPipe();
  void TestUpdate();
  void TestUpdateBounds();
  void TestSetInput();
  void TestSetNthInput();
	

  bool result;
};

#endif

