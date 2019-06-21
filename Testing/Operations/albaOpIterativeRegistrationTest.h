/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpIterativeRegistrationTest
 Authors: Alberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpIterativeRegistrationTest_H
#define CPP_UNIT_albaOpIterativeRegistrationTest_H

#include "albaTest.h"
using namespace std;
 
class albaOpIterativeRegistrationTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaOpIterativeRegistrationTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  // CPPUNIT_TEST( TestOnEvent );
  CPPUNIT_TEST( TestCopy);
  CPPUNIT_TEST( TestAccept);
  // CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestVmeAccept );
  CPPUNIT_TEST( TestOpDoUndo );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOnEvent();
  void TestCopy();
  void TestAccept();
  void TestOpRun();
  void TestVmeAccept();
  void TestOpDoUndo();
};

#endif
