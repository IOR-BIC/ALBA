/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPadMouseRemoteTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceButtonsPadMouseRemoteTest_H__
#define __CPP_UNIT_mafDeviceButtonsPadMouseRemoteTest_H__

#include "mafTest.h"

class mafDeviceButtonsPadMouseRemoteTest : public mafTest
{
  public:
  
  CPPUNIT_TEST_SUITE( mafDeviceButtonsPadMouseRemoteTest );
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
