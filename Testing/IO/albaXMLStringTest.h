/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaXMLStringTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaXMLStringTest_H__
#define __CPP_UNIT_albaXMLStringTest_H__

#include "albaTest.h"

class albaXMLString;

class albaXMLStringTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( albaXMLStringTest );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAllConstructors );
  CPPUNIT_TEST( TestAppend );
  CPPUNIT_TEST( TestErase );
  CPPUNIT_TEST( TestBegin );
  CPPUNIT_TEST( TestEnd );
  CPPUNIT_TEST( TestSize );
  CPPUNIT_TEST( TestGetCStr );
  CPPUNIT_TEST( TestAllOperators );
	CPPUNIT_TEST_SUITE_END();

protected:

  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAllConstructors();
  void TestAppend();
  void TestErase();
  void TestBegin();
  void TestEnd();
  void TestSize();
  void TestGetCStr();
  void TestAllOperators();
};

#endif 