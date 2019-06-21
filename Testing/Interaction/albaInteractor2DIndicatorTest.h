/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractor2DIndicatorTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaInteractor2DIndicatorTest_H__
#define __CPP_UNIT_albaInteractor2DIndicatorTest_H__

#include "albaTest.h"

class albaGUIFrame;

class albaInteractor2DIndicatorTest : public albaTest
{
public:

	CPPUNIT_TEST_SUITE( albaInteractor2DIndicatorTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestStaticAllocation );
  CPPUNIT_TEST( TestIsDisableUndoAndOkCancel );
  CPPUNIT_TEST( TestUndoMeasure );
  CPPUNIT_TEST( TestSetLabel );
  CPPUNIT_TEST( TestGetLabel );
  CPPUNIT_TEST( TestGetRegisterMeasure );
  CPPUNIT_TEST( TestSizeMeasureVector );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestDynamicAllocation();
  void TestStaticAllocation();
  void TestIsDisableUndoAndOkCancel();
  void TestUndoMeasure();
  void TestRemoveMeter();
  void TestSetLabel();
  void TestGetLabel();
  void TestGetRegisterMeasure();
  void TestSizeMeasureVector();
};

#endif
