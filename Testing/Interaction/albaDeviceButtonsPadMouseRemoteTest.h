/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDeviceButtonsPadMouseRemoteTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDeviceButtonsPadMouseRemoteTest_H__
#define __CPP_UNIT_albaDeviceButtonsPadMouseRemoteTest_H__

#include "albaTest.h"

class albaDeviceButtonsPadMouseRemoteTest : public albaTest
{
  public:
  
  CPPUNIT_TEST_SUITE( albaDeviceButtonsPadMouseRemoteTest );
    CPPUNIT_TEST(TestFixture);
    CPPUNIT_TEST(TestConstructorDestructor);
    CPPUNIT_TEST(TestOnEventMouse2DMove);
    CPPUNIT_TEST(TestOnEventButtonDown);
    CPPUNIT_TEST(TestOnEventMouseDClick);
    CPPUNIT_TEST(TestOnEventButtonUp);
    CPPUNIT_TEST(TestOnEventViewSelect);
    CPPUNIT_TEST(TestOnEventViewDelete);
    CPPUNIT_TEST(TestOnEventMouseCharEvent);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestOnEventMouse2DMove();
  void TestOnEventButtonDown();
  void TestOnEventMouseDClick();
  void TestOnEventButtonUp();
  void TestOnEventViewSelect();
  void TestOnEventViewDelete();
  void TestOnEventMouseCharEvent();
};

#endif
