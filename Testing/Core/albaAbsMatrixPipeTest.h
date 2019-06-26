/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAbsMatrixPipeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAbsMatrixPipeTest_H__
#define __CPP_UNIT_albaAbsMatrixPipeTest_H__

#include "albaTest.h"

/** Test for albaAbsMatrixPipe; Use this suite to trace memory problems */
class albaAbsMatrixPipeTest : public albaTest
{
public: 
  
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaAbsMatrixPipeTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestSetVME);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestSetVME();

  bool result;
};


#endif
