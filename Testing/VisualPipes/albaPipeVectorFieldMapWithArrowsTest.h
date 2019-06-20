/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorFieldMapWithArrowsTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeVectorFieldMapWithArrowsTest_H__
#define __CPP_UNIT_albaPipeVectorFieldMapWithArrowsTest_H__

#include "albaTest.h"

class albaPipeVectorFieldMapWithArrowsTest : public albaTest
{
public: 
	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeVectorFieldMapWithArrowsTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestCreate );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestCreate();
};

#endif
