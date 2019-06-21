/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaApplicationLayoutTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmaApplicationLayoutTest_H__
#define __CPP_UNIT_mmaApplicationLayoutTest_H__

#include "albaTest.h"


/** Test for albaMatrix; Use this suite to trace memory problems */
class mmaApplicationLayoutTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mmaApplicationLayoutTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestEquals();
  void TestDeepCopy();
	
  wxFrame *m_Toplevel;
  bool result;
};

#endif

