/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaToolSelectPointTest
 Authors: Stefano Perticoni, Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaToolSelectPointTest_H__
#define __CPP_UNIT_albaToolSelectPointTest_H__

#include "albaTest.h"

class albaToolSelectPointTest : public albaTest
{
public:
  CPPUNIT_TEST_SUITE(albaToolSelectPointTest);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetGetCtrlModifier);
  CPPUNIT_TEST(TestEventsWithoutRenderer);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestConstructorDestructor();
  void TestSetGetCtrlModifier();
  void TestEventsWithoutRenderer();
};

#endif
