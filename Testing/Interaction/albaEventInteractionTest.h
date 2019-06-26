/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventInteractionTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaEventInteractionTest_H__
#define __CPP_UNIT_albaEventInteractionTest_H__

#include "albaTest.h"

/** 
  class name albaEventInteraction 
  Use this suite to trace memory problems 
*/
class albaEventInteractionTest : public albaTest
{
public: 
  /** CPPUNIT test suite */
  CPPUNIT_TEST_SUITE( albaEventInteractionTest );
  /** CPPUNIT test : static allocation */
  CPPUNIT_TEST(TestStaticAllocation);
  /** CPPUNIT test : dynamic allocation */
  CPPUNIT_TEST(TestDynamicAllocation);
  /** CPPUNIT test : set/get position */
  CPPUNIT_TEST(TestSet2DPosition_Get2DPosition);
  /** CPPUNIT test : set/get xyflag */
  CPPUNIT_TEST(TestSetXYFlag_GetXYFlag);
  /** CPPUNIT test : set/get button */
  CPPUNIT_TEST(TestSetButton_GetButton);
  /** CPPUNIT test : set/get key */
  CPPUNIT_TEST(TestSetKey_GetKey);
  /** CPPUNIT test : set/get matrix */
  CPPUNIT_TEST(TestSetMatrix_GetMatrix);
  /** CPPUNIT test : set/get modifier */
  CPPUNIT_TEST(TestSetModifier_GetModifier);
  /** CPPUNIT test : set/get modifiers */
  CPPUNIT_TEST(TestSetModifiers_GetModifiers);
  /** CPPUNIT test : deepcopy of the object */
  CPPUNIT_TEST(TestDeepCopy);
  /** CPPUNIT end test suite */
  CPPUNIT_TEST_SUITE_END();
  

private:
  /** static allocation */
  void TestStaticAllocation();
  /** dynamic allocation */
  void TestDynamicAllocation();
  /** set/get position */
  void TestSet2DPosition_Get2DPosition();
  /** set/get xyflag */
  void TestSetXYFlag_GetXYFlag();
  /** set/get button */
  void TestSetButton_GetButton();
  /** set/get key */
  void TestSetKey_GetKey();
  /** set/get matrix */
  void TestSetMatrix_GetMatrix();
  /** set/get modifier */
  void TestSetModifier_GetModifier();
  /** set/get modifiers */
  void TestSetModifiers_GetModifiers();
  /** deepcopy of the object */
  void TestDeepCopy();

  enum
  {
    ID_MODIFIER_1 = 1,
    ID_MODIFIER_2,
  };
  enum
  {
    ID_KEY_1,
    ID_KEY_2,
  };
  enum
  {
    ID_BUTTON_LEFT,
    ID_BUTTON_RIGHT,
  };
};

#endif
