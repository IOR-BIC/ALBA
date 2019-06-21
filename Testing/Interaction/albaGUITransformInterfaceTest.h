/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformInterfaceTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUITransformInterfaceTest_H__
#define __CPP_UNIT_albaGUITransformInterfaceTest_H__

#include "albaTest.h"

/** albaGUITransformInterfaceTest */
class albaGUITransformInterfaceTest : public albaTest
{
public: 

	CPPUNIT_TEST_SUITE( albaGUITransformInterfaceTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST(TestDynamicAllocation);
  CPPUNIT_TEST(TestStaticAllocation);
  CPPUNIT_TEST(TestTestModeOn);
  CPPUNIT_TEST(TestTestModeOff);
  CPPUNIT_TEST(TestSetRefSys);
  CPPUNIT_TEST_SUITE_END();

private:

  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestTestModeOn();
  void TestTestModeOff();
  void TestSetRefSys();

};

#endif
