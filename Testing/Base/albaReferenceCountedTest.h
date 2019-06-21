/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaReferenceCountedTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaReferenceCountedTest_H__
#define __CPP_UNIT_albaReferenceCountedTest_H__

#include "albaTest.h"

//forward reference
class albaReferenceCounted;

/** Test for albaMatrix; Use this suite to trace memory problems */
class albaReferenceCountedTest : public albaTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaReferenceCountedTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestRegister);
  CPPUNIT_TEST(TestGetReferenceCount);
  CPPUNIT_TEST(TestDelete);
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestRegister();
  void TestGetReferenceCount();
  void TestDelete();
};

#endif
