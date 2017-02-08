/*=========================================================================

 Program: MAF2
 Module: mafInteractor2DDistanceTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafInteractor2DDistanceTest_H__
#define __CPP_UNIT_mafInteractor2DDistanceTest_H__

#include "mafTest.h"

class mafGUIFrame;

class mafInteractor2DDistanceTest : public mafTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafInteractor2DDistanceTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestSetMeasureType );
  CPPUNIT_TEST( TestGenerateHistogram  );
  CPPUNIT_TEST( TestSetManualDistance1  );
  CPPUNIT_TEST( TestSetManualDistance2  );
  CPPUNIT_TEST( TestIsDisableUndoAndOkCancel  );
  CPPUNIT_TEST( TestGetLastDistance  );
  CPPUNIT_TEST( TestSetLabel  );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestSetMeasureType();
  void TestGenerateHistogram();
  void TestSetManualDistance1();
  void TestSetManualDistance2();
  void TestIsDisableUndoAndOkCancel();
  void TestGetLastDistance();
  void TestSetLabel();

  mafGUIFrame *m_Win;
};

#endif
