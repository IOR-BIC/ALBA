/*=========================================================================

 Program: MAF2
 Module: mafGUITransformSlidersTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGUITransformSlidersTest_H__
#define __CPP_UNIT_mafGUITransformSlidersTest_H__

#include "mafTest.h"


class mafGUITransformSlidersTest : public mafTest
{
public:

  CPPUNIT_TEST_SUITE( mafGUITransformSlidersTest );
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
