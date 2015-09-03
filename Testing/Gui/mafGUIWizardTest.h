/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUIWizardTest_H__
#define __CPP_UNIT_mafGUIWizardTest_H__

#include "mafTest.h"

class mafGUIFrame;

class mafGUIWizardTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUIWizardTest );
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

  mafGUIFrame *m_Win;
};

#endif
