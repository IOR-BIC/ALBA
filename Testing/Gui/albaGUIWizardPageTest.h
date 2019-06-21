/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUIWizardPageTest_H__
#define __CPP_UNIT_albaGUIWizardPageTest_H__

#include "albaTest.h"

class albaGUIFrame;

class albaGUIWizardPageTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGUIWizardPageTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAddGuiLowerLeft );
  CPPUNIT_TEST( TestAddGuiLowerRight );
  CPPUNIT_TEST( TestAddGuiLowerCenter );
  CPPUNIT_TEST( TestRemoveGuiLowerLeft );
  CPPUNIT_TEST( TestRemoveGuiLowerRight );
  CPPUNIT_TEST( TestRemoveGuiLowerCenter );
  CPPUNIT_TEST( TestSetNextPage );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestDynamicAllocation();
  void TestAddGuiLowerLeft();
  void TestAddGuiLowerRight();
  void TestAddGuiLowerCenter();
  void TestRemoveGuiLowerLeft();
  void TestRemoveGuiLowerRight();
  void TestRemoveGuiLowerCenter();
  void TestSetNextPage();

  bool m_Result;

  albaGUIFrame *m_Win;

};

#endif
