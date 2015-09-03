/*=========================================================================

 Program: MAF2
 Module: mafGUIGizmoInterfaceTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNITmafGUIGizmoInterfaceTest_H__
#define __CPP_UNITmafGUIGizmoInterfaceTest_H__

#include "mafTest.h"

/** mafGUIGizmoInterfaceTest */
class mafGUIGizmoInterfaceTest : public mafTest
{
public: 

  CPPUNIT_TEST_SUITE( mafGUIGizmoInterfaceTest );
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
