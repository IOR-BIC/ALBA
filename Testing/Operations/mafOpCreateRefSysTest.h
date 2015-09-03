/*=========================================================================

 Program: MAF2
 Module: mafOpCreateRefSysTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafOpCreateRefSysTest_H__
#define __CPP_UNIT_mafOpCreateRefSysTest_H__

#include "mafTest.h"

class mafOpCreateRefSys;

/** Test for mafOpCreateRefSys; Use this suite to trace memory problems */
class mafOpCreateRefSysTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafOpCreateRefSysTest );
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

  mafOpCreateRefSys *m_CreateRefSys;
};

#endif
