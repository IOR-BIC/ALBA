/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpTransformOldTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaOpTransformOldTest_H__
#define __CPP_UNIT_albaOpTransformOldTest_H__

#include "albaTest.h"

/** Test for albaOpTransformOld; Use this suite to trace memory problems */
class albaOpTransformOldTest : public albaTest
{
public: 
  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( albaOpTransformOldTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAccept );
  CPPUNIT_TEST( TestOnEventGizmoTranslate );
  CPPUNIT_TEST( TestOnEventGizmoRotate );
  CPPUNIT_TEST( TestOnEventGizmoScale );
  CPPUNIT_TEST( TestOnEventGuiTransform );
  CPPUNIT_TEST( TestOnEventGuiSaveRestorePose );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST( TestOpUndo );
  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestAccept();
  void TestOnEventGizmoTranslate();
  void TestOnEventGizmoRotate();
  void TestOnEventGizmoScale();
  void TestOnEventGuiTransform();
  void TestReset();
  void TestOnEventGuiSaveRestorePose();
  void TestOpUndo();
};

#endif
