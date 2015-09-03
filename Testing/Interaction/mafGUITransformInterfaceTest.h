/*=========================================================================

 Program: MAF2
 Module: mafGUITransformInterfaceTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUITransformInterfaceTest_H__
#define __CPP_UNIT_mafGUITransformInterfaceTest_H__

#include "mafTest.h"

/** mafGUITransformInterfaceTest */
class mafGUITransformInterfaceTest : public mafTest
{
public: 

	CPPUNIT_TEST_SUITE( mafGUITransformInterfaceTest );
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
