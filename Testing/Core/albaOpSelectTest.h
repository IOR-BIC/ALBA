/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSelectTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpSelectTest_H__
#define __CPP_UNIT_albaOpSelectTest_H__

#include "albaTest.h"

//forward reference
class albaOpSelect;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpSelectTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpSelectTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestAccept);
  CPPUNIT_TEST(TestSetInput);
  CPPUNIT_TEST(TestSetNewSel);
  CPPUNIT_TEST(TestCopy);
  CPPUNIT_TEST(TestOpDo);
  CPPUNIT_TEST(TestOpUndo);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestDynamicAllocation();
  void TestAccept();
  void TestSetInput();
  void TestSetNewSel();
  void TestCopy();
  void TestOpDo();
  void TestOpUndo();

  albaOpSelect *m_OpSelect;
};

#endif
