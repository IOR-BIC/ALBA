/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor2DAngleTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractor2DAngleTest_H__
#define __CPP_UNIT_albaInteractor2DAngleTest_H__

#include "albaTest.h"

class albaGUIFrame;

class albaInteractor2DAngleTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaInteractor2DAngleTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestSetMeasureType );
  CPPUNIT_TEST( TestSetManualDistance  );
  CPPUNIT_TEST( TestIsDisableUndoAndOkCancel  );
  CPPUNIT_TEST( TestGetLastAngle  );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestSetMeasureType();
  void TestSetManualDistance();
  void TestIsDisableUndoAndOkCancel();
  void TestGetLastAngle();

  albaGUIFrame *m_Win;
};

#endif
