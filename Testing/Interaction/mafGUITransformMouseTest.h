/*=========================================================================

 Program: MAF2
 Module: mafGUITransformMouseTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUITransformMouseTest_H__
#define __CPP_UNIT_mafGUITransformMouseTest_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafTest.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIFrame;

class mafGUITransformMouseTest : public mafTest
{
public:

  /** Executed before each test */
  void BeforeTest();

  /** Executed after each test */
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGUITransformMouseTest );
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

  mafGUIFrame *m_Win;

};
#endif
