/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardPageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUIWizardPageTest_H__
#define __CPP_UNIT_mafGUIWizardPageTest_H__

#include "mafTest.h"

class mafGUIFrame;

class mafGUIWizardPageTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUIWizardPageTest );
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

  mafGUIFrame *m_Win;

};

#endif
