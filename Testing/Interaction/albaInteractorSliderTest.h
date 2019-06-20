/*=========================================================================
Program:   ALBA
Module:    albaInteractorSliderTest.cpp
Language:  C++
Date:      $Date: 2019-04-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __CPP_UNIT_albaInteractorSliderTest_H__
#define __CPP_UNIT_albaInteractorSliderTest_H__

#include "albaTest.h"

class albaGUIFrame;

class albaInteractorSliderTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaInteractorSliderTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestSetRange );
  CPPUNIT_TEST( TestSetSteps  );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestSetRange();
  void TestSetSteps();

  albaGUIFrame *m_Win;
};

#endif
