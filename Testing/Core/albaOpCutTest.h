/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCutTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpCutTest_H__
#define __CPP_UNIT_albaOpCutTest_H__

#include "albaTest.h"

//forward reference
class albaOpCut;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaOpCutTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpCutTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestOpDo );
  CPPUNIT_TEST( TestOpUndo );
  CPPUNIT_TEST( TestOpDoVMETimeVarying );
  CPPUNIT_TEST( TestOpUndoVMETimeVarying );
  CPPUNIT_TEST( TestOpUndoVMEWithChildren );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestOpDo();
  void TestOpUndo();
  void TestOpDoVMETimeVarying();
  void TestOpUndoVMETimeVarying();
  void TestOpUndoVMEWithChildren();

  albaOpCut *m_OpCut;
};


#endif
