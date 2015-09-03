/*=========================================================================

 Program: MAF2
 Module: mafReferenceCountedTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafReferenceCountedTest_H__
#define __CPP_UNIT_mafReferenceCountedTest_H__

#include "mafTest.h"

//forward reference
class mafReferenceCounted;

/** Test for mafMatrix; Use this suite to trace memory problems */
class mafReferenceCountedTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mafReferenceCountedTest );
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
