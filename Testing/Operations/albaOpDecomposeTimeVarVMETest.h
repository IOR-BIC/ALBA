/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDecomposeTimeVarVMETest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_ALBAOPDECOMPOSETIMEVARVMETEST_H
#define CPP_UNIT_ALBAOPDECOMPOSETIMEVARVMETEST_H

#include "albaTest.h"

class albaOpDecomposeTimeVarVMETest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaOpDecomposeTimeVarVMETest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestModeTimeStampsVME );
  CPPUNIT_TEST( TestModeIntervalVME );
  CPPUNIT_TEST( TestModePeriodicityVME );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestConstructor();
  void TestModeTimeStampsVME();
  void TestModeIntervalVME();
  void TestModePeriodicityVME();
};


#endif
