/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformMouseTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUITransformMouseTest_H__
#define __CPP_UNIT_albaGUITransformMouseTest_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaTest.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUIFrame;

class albaGUITransformMouseTest : public albaTest
{
public:

  /** Executed before each test */
  void BeforeTest();

  /** Executed after each test */
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGUITransformMouseTest );
  CPPUNIT_TEST(TestFixture);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestEnableWidgets);
  CPPUNIT_TEST(TestAttachInteractorToVme);
  CPPUNIT_TEST(TestDetachInteractorFromVme);
  CPPUNIT_TEST(TestCreateBehavior);
  CPPUNIT_TEST(TestOnEvent);
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestEnableWidgets();
  void TestAttachInteractorToVme();
  void TestDetachInteractorFromVme();
  void TestCreateBehavior();
  void TestOnEvent();

  albaGUIFrame *m_Win;

};
#endif
