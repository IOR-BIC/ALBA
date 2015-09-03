/*=========================================================================

 Program: MAF2
 Module: mafDeviceButtonsPadMouseTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDeviceButtonsPadMouseTest_H__
#define __CPP_UNIT_mafDeviceButtonsPadMouseTest_H__

#include "mafTest.h"

class mafDeviceButtonsPadMouseTest : public mafTest
{
  public:

  CPPUNIT_TEST_SUITE( mafDeviceButtonsPadMouseTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestConstructorDestructor);
  CPPUNIT_TEST(TestSetGetLastPosition);
  CPPUNIT_TEST(TestGetRenderer);
  CPPUNIT_TEST(TestGetView);
  CPPUNIT_TEST(TestGetInteractor);
  CPPUNIT_TEST(TestGetRWI);
  CPPUNIT_TEST(TestSetUpdateRWIDuringMotion);
  CPPUNIT_TEST(TestUpdateRWIDuringMotionOnOff);
  CPPUNIT_TEST(TestIsUpdateRWIDuringMotion);
  CPPUNIT_TEST(TestDisplayToNormalizedDisplay);
  CPPUNIT_TEST(TestNormalizedDisplayToDisplay);
  CPPUNIT_TEST(TestCollaborate);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
  void TestFixture();
  void TestConstructorDestructor();
  void TestSetGetLastPosition();
  void TestGetRenderer();
  void TestGetView();
  void TestGetInteractor();
  void TestGetRWI();
  void TestSetUpdateRWIDuringMotion();
  void TestUpdateRWIDuringMotionOnOff();
  void TestIsUpdateRWIDuringMotion();
  void TestDisplayToNormalizedDisplay();
  void TestNormalizedDisplayToDisplay();
  void TestCollaborate();

};

#endif
