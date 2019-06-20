/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUITransformSlidersTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGUITransformSlidersTest_H__
#define __CPP_UNIT_albaGUITransformSlidersTest_H__

#include "albaTest.h"


class albaGUITransformSlidersTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaGUITransformSlidersTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestSetAbsPose1 );
  CPPUNIT_TEST( TestSetAbsPose2 );
  CPPUNIT_TEST( TestSetAbsPose3 );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST( TestOnEvent );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestSetAbsPose1();
  void TestSetAbsPose2();
  void TestSetAbsPose3();
  void TestReset();
  void TestOnEvent();

};

#endif
