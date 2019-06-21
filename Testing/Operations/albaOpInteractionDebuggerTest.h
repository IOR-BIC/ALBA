/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpInteractionDebuggerTest
 Authors: Alberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpInteractionDebuggerTest_H
#define CPP_UNIT_albaOpInteractionDebuggerTest_H

#include "albaTest.h"
using namespace std;
 
class albaOpInteractionDebuggerTest : public albaTest
{

public:

  CPPUNIT_TEST_SUITE( albaOpInteractionDebuggerTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  // CPPUNIT_TEST( TestOnEvent );
  CPPUNIT_TEST( TestCopy);
  CPPUNIT_TEST( TestAccept);
  // CPPUNIT_TEST( TestOpRun );
  CPPUNIT_TEST( TestConstrainAccept );
  CPPUNIT_TEST( TestOpDo );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestDynamicAllocation();
  void TestOnEvent();
  void TestCopy();
  void TestAccept();
  void TestOpRun();
  void TestConstrainAccept();
  void TestOpDo();
};

#endif
