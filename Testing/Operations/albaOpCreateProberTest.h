/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateProberTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpCreateProberTest_H__
#define __CPP_UNIT_albaOpCreateProberTest_H__

#include "albaTest.h"

class albaOpCreateProber;

/** Test for albaOpCreateProber; Use this suite to trace memory problems */
class albaOpCreateProberTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpCreateProberTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  
  //ToDo: add your test here...
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestOpDo );

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  
  // ToDo: add your test methods here...
  void TestAccept();
  void TestOpRun();
  void TestOpDo();

  bool result;

  albaOpCreateProber *m_CreateProber;
};

#endif
