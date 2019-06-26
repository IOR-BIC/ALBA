/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmuDOMTreeErrorReporterTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDirectoryTest_H__
#define __CPP_UNIT_albaDirectoryTest_H__

#include "albaTest.h"

class mmuDOMTreeErrorReporter;

class mmuDOMTreeErrorReporterTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

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
