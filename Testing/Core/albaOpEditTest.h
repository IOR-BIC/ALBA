/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpEditTest_H__
#define __CPP_UNIT_albaOpEditTest_H__

#include "albaTest.h"

//forward reference
class albaOpEdit;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpEditTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpEditTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestClipboardIsEmpty );
  CPPUNIT_TEST( TestClipboardClear );
  CPPUNIT_TEST( TestClipboardBackupRestore );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestClipboardIsEmpty();
  void TestClipboardClear();
  void TestClipboardBackupRestore();

  albaOpEdit *m_OpEdit;
};


#endif
