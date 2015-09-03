/*=========================================================================

 Program: MAF2
 Module: mafDynamicLoaderTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDynamicLoaderTest_H__
#define __CPP_UNIT_mafDynamicLoaderTest_H__

#include "mafTest.h"

/** Test for mafDynamicLoader; Use this suite to trace memory problems */
class mafDynamicLoaderTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafDynamicLoaderTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST(TestOpenLibrary);
  CPPUNIT_TEST(TestCloseLibrary);
  CPPUNIT_TEST(TestGetSymbolAddress);
  CPPUNIT_TEST(TestLibPrefix);
  CPPUNIT_TEST(TestLibExtension);
	CPPUNIT_TEST(TestLastError);
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
	void TestOpenLibrary();
	void TestCloseLibrary();
	void TestGetSymbolAddress();
	void TestLibPrefix();
	void TestLibExtension();
	void TestLastError();

	bool result;
};


#endif
