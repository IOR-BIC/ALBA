/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUIWizardTest_H__
#define __CPP_UNIT_albaGUIWizardTest_H__

#include "albaTest.h"

class albaGUIFrame;

class albaGUIWizardTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGUIWizardTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestSetFirstPage );
  CPPUNIT_TEST( TestEnableChangePageOn );
  CPPUNIT_TEST( TestEnableChangePageOff );
  CPPUNIT_TEST( TestRun );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestDynamicAllocation();
  void TestSetFirstPage();
  void TestEnableChangePageOn();
  void TestEnableChangePageOff();
  void TestRun();

  bool m_Result;

  albaGUIFrame *m_Win;
};

#endif
