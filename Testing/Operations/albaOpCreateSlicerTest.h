/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSlicerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpCreateSlicerTest_H__
#define __CPP_UNIT_albaOpCreateSlicerTest_H__

#include "albaTest.h"

class albaOpCreateSlicer;

/** Test for albaOpCreateSlicer; Use this suite to trace memory problems */
class albaOpCreateSlicerTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpCreateSlicerTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestOpDo );

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  
  void TestAccept();
  void TestOpRun();
  void TestOpDo();

  bool result;

  albaOpCreateSlicer *m_CreateSlicer;
};

#endif
