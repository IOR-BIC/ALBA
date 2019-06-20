/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIGizmoInterfaceTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNITalbaGUIGizmoInterfaceTest_H__
#define __CPP_UNITalbaGUIGizmoInterfaceTest_H__

#include "albaTest.h"

/** albaGUIGizmoInterfaceTest */
class albaGUIGizmoInterfaceTest : public albaTest
{
public: 

  CPPUNIT_TEST_SUITE( albaGUIGizmoInterfaceTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST(TestGetGui);
  CPPUNIT_TEST(TestEnableWidgets);
  CPPUNIT_TEST(TestTestModeOn);
  CPPUNIT_TEST(TestTestModeOff);
  CPPUNIT_TEST(TestGetTestMode);
  CPPUNIT_TEST_SUITE_END();

  
private:

  void TestFixture();
  void TestConstructorDestructor();
  void TestOnEvent();
  void TestGetGui();
  void TestEnableWidgets();
  void TestTestModeOn();
  void TestTestModeOff();
  void TestGetTestMode();

};

#endif
