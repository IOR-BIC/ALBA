/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRotateTranslateBehaviorTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaRotateTranslateBehaviorTest_H__
#define __CPP_UNIT_albaRotateTranslateBehaviorTest_H__

#include "albaTest.h"

/** Test of albaRotateTranslateBehavior, albaProsthesisBehavior and albaCutPlaneBehavior */
class albaRotateTranslateBehaviorTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaRotateTranslateBehaviorTest );
  CPPUNIT_TEST( TestRotateTranslateButtons );
  CPPUNIT_TEST( TestRefSysVME );
  CPPUNIT_TEST( TestProsthesisButtons );
  CPPUNIT_TEST( TestCutPlaneButtons );
  CPPUNIT_TEST( TestCutPlaneClicks );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestRotateTranslateButtons();
  void TestRefSysVME();
  void TestProsthesisButtons();
  void TestCutPlaneButtons();
  void TestCutPlaneClicks();
};

#endif
