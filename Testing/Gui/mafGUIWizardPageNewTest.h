/*=========================================================================

 Program: MAF2
 Module: mafGUIWizardPageNewTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUIWizardPageNewTest_H__
#define __CPP_UNIT_mafGUIWizardPageNewTest_H__

#include "mafTest.h"

class mafGUIFrame;

class mafGUIWizardPageNewTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUIWizardPageNewTest );
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

  mafGUIFrame *m_Win;

};

#endif
