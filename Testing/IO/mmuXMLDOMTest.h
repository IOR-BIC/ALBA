/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmuXMLDOMTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmuXMLDOMTest_H__
#define __CPP_UNIT_mmuXMLDOMTest_H__

#include "albaTest.h"

class mmuXMLDOM;

class mmuXMLDOMTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( mmuXMLDOMTest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestFixture();
	void TestStaticAllocation();
	void TestDynamicAllocation();

  
};

#endif
