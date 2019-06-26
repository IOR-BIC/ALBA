/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIWizardPageNewTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUIWizardPageNewTest_H__
#define __CPP_UNIT_albaGUIWizardPageNewTest_H__

#include "albaTest.h"

class albaGUIFrame;

class albaGUIWizardPageNewTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGUIWizardPageNewTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAddGuiLowerLeft );
  CPPUNIT_TEST( TestAddGuiLowerRight );
  CPPUNIT_TEST( TestAddGuiLowerCenter );
  
  CPPUNIT_TEST( TestAddGuiLowerUnderLeft );
  CPPUNIT_TEST( TestAddGuiLowerUnderCenter );
  CPPUNIT_TEST( TestRemoveGuiLowerLeft );
  CPPUNIT_TEST( TestRemoveGuiLowerUnderLeft );

  CPPUNIT_TEST( TestSetNextPage );
  CPPUNIT_TEST( TestSetGetZCropBounds );
  CPPUNIT_TEST( TestUpdateWindowing );
  CPPUNIT_TEST( TestUpdateActor );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestDynamicAllocation();
  void TestAddGuiLowerLeft();
  void TestAddGuiLowerRight();
  void TestAddGuiLowerCenter();

  void TestAddGuiLowerUnderLeft();
  void TestAddGuiLowerUnderCenter();

  void TestRemoveGuiLowerLeft();
  void TestRemoveGuiLowerUnderLeft();

  void TestSetNextPage();
  void TestSetGetZCropBounds();
  void TestUpdateWindowing();
  void TestUpdateActor();

  bool m_Result;

  albaGUIFrame *m_Win;

};

#endif
