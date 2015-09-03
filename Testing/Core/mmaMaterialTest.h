/*=========================================================================

 Program: MAF2
 Module: mmaMaterialTest
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mmaMaterialTest_H__
#define __CPP_UNIT_mmaMaterialTest_H__

#include "mafTest.h"

/** Test for mmaMaterial. Use this suite to trace memory problems */
class mmaMaterialTest : public mafTest
{
public: 

  // CPPUNIT test suite
  CPPUNIT_TEST_SUITE( mmaMaterialTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestSetMaterialTexture);
  CPPUNIT_TEST(TestSetMaterialTexture2);
  CPPUNIT_TEST(TestDeepCopy);
  CPPUNIT_TEST(TestEquals);
  CPPUNIT_TEST(TestUpdateFromLut);
  CPPUNIT_TEST(TestUpdateProp);

  CPPUNIT_TEST_SUITE_END();

private:
  void TestFixture();
  void TestStaticAllocation();
  void TestDynamicAllocation();
  void TestSetMaterialTexture();
  void TestSetMaterialTexture2();
  void TestDeepCopy();
  void TestEquals();
  void TestUpdateFromLut();
  void TestUpdateProp();

  bool result;
};


#endif
