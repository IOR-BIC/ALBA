/*=========================================================================

 Program: MAF2
 Module: mafInteractor2DAngleTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractor2DAngleTest_H__
#define __CPP_UNIT_mafInteractor2DAngleTest_H__

#include "mafTest.h"

class mafGUIFrame;

class mafInteractor2DAngleTest : public mafTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafInteractor2DAngleTest );
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

  mafGUIFrame *m_Win;
};

#endif
