/*=========================================================================

 Program: MAF2
 Module: mafOpInteractionDebuggerTest
 Authors: Alberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_mafOpInteractionDebuggerTest_H
#define CPP_UNIT_mafOpInteractionDebuggerTest_H

#include "mafTest.h"
using namespace std;
 
class mafOpInteractionDebuggerTest : public mafTest
{

public:

  CPPUNIT_TEST_SUITE( mafOpInteractionDebuggerTest );
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
