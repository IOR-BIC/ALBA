/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDeviceTest_H__
#define __CPP_UNIT_albaDeviceTest_H__

#include "albaTest.h"

class albaDeviceTest : public albaTest
{
  public:

  CPPUNIT_TEST_SUITE( albaDeviceTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetName);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestGetViewAndRenderer);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestSetName();
  void TestOnEvent();
  void TestGetViewAndRenderer();
};

#endif