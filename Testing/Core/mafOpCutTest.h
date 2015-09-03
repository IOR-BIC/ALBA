/*=========================================================================

 Program: MAF2
 Module: mafOpCutTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpCutTest_H__
#define __CPP_UNIT_mafOpCutTest_H__

#include "mafTest.h"

//forward reference
class mafOpCut;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafOpCutTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpCutTest );
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

  mafOpCut *m_OpCut;
};


#endif
