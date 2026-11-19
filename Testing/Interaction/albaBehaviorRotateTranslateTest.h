/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorRotateTranslateTest
 Authors: Gianluigi Crimi

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaBehaviorRotateTranslateTest_H__
#define __CPP_UNIT_albaBehaviorRotateTranslateTest_H__

#include "albaTest.h"

/** Test of albaBehaviorRotateTranslate, albaBehaviorProsthesis and albaBehaviorCutPlane */
class albaBehaviorRotateTranslateTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaBehaviorRotateTranslateTest );
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
