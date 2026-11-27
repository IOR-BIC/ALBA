/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRotateTranslateGUITest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaRotateTranslateGUITest_H__
#define __CPP_UNIT_albaRotateTranslateGUITest_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaTest.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIFrame;
class albaBehaviorRotateTranslate;
class albaVME;

class albaRotateTranslateGUITest : public albaTest
{
public:

  /** Executed before each test */
  void BeforeTest();

  /** Executed after each test */
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaRotateTranslateGUITest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestEnableWidgets);
  CPPUNIT_TEST(TestTransformBehaviors);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestEnableWidgets();
  void TestTransformBehaviors();
  void TestOnEvent();

  /** Create the behavior that an operation would give to the GUI */
  albaBehaviorRotateTranslate *CreateBehavior(albaVME *vme);

  albaGUIFrame *m_Win;

};
#endif
