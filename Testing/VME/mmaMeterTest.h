/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaMeterTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmaMeterTest_H__
#define __CPP_UNIT_mmaMeterTest_H__

#include "albaTest.h"

/** Test for albaMatrix; Use this suite to trace memory problems */
class mmaMeterTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mmaMeterTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestPrint );
  CPPUNIT_TEST( TestDeepCopy );
	CPPUNIT_TEST( TestEquals );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestPrint();
  void TestDeepCopy();
	void TestEquals();

	bool m_Result;
};

#endif
