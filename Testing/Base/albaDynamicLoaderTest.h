/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDynamicLoaderTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDynamicLoaderTest_H__
#define __CPP_UNIT_albaDynamicLoaderTest_H__

#include "albaTest.h"

/** Test for albaDynamicLoader; Use this suite to trace memory problems */
class albaDynamicLoaderTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaDynamicLoaderTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST(TestOpenLibrary);
  CPPUNIT_TEST(TestCloseLibrary);
  CPPUNIT_TEST(TestGetSymbolAddress);
	CPPUNIT_TEST(TestCallFunction);
  CPPUNIT_TEST(TestLibPrefix);
  CPPUNIT_TEST(TestLibExtension);
	CPPUNIT_TEST(TestLastError);
  
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
	void TestOpenLibrary();
	void TestCloseLibrary();
	void TestGetSymbolAddress();
	void TestCallFunction();
	void TestLibPrefix();
	void TestLibExtension();
	void TestLastError();

	bool result;
};


#endif
