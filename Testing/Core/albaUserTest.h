/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaUserTest
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaUserTest_H__
#define __CPP_UNIT_albaUserTest_H__

#include "albaTest.h"

/** Test for albaUser; Use this suite to trace memory problems */
class albaUserTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaUserTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);

  CPPUNIT_TEST(TestSetCredentials);
  CPPUNIT_TEST(TestCheckUserCredentials);
  CPPUNIT_TEST_SUITE_END();
  

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();

  void TestSetCredentials();
  void TestCheckUserCredentials();

};


#endif
