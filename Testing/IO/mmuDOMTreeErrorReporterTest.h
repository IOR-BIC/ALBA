/*=========================================================================

 Program: MAF2
 Module: mmuDOMTreeErrorReporterTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDirectoryTest_H__
#define __CPP_UNIT_mafDirectoryTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class mmuDOMTreeErrorReporter;

class mmuDOMTreeErrorReporterTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

	CPPUNIT_TEST_SUITE( mmuDOMTreeErrorReporterTest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );

  CPPUNIT_TEST( TestWarning );
  CPPUNIT_TEST( TestError );
  CPPUNIT_TEST( TestFatalError );
  CPPUNIT_TEST( TestResetErrors );
	CPPUNIT_TEST_SUITE_END();

	mmuDOMTreeErrorReporter *m_Directory;

protected:
	void TestFixture();
	void TestStaticAllocation();
	void TestDynamicAllocation();
  void TestWarning();
  void TestError();
  void TestFatalError();
  void TestResetErrors();
  
};

#endif
