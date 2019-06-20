/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATextActorMeterTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBATextActorMetertest_H__
#define __CPP_UNIT_vtkALBATextActorMetertest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkALBATextActorMeter;

//------------------------------------------------------------------------------
// Test class for vtkALBATextActorMeter
//------------------------------------------------------------------------------
class vtkALBATextActorMeterTest : public albaTest
{
  public:
		// Executed before each test
		void BeforeTest();

		// Executed after each test
		void AfterTest();

    CPPUNIT_TEST_SUITE( vtkALBATextActorMeterTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestTextPos1 );
    CPPUNIT_TEST( TestTextPos2 );
    CPPUNIT_TEST( TestTextColor );
    CPPUNIT_TEST( TestPrintSelf );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestTextPos1();
    void TestTextPos2();
    void TestTextColor();
    void TestPrintSelf();
    /*void TestBackgroundColor();
    void TestBackgroundVisibility();
    void TestScale();*/

    //accessories
    void SetText(vtkALBATextActorMeter *actor, const char *text, double position[3]);
    void RenderData(vtkActor2D *actor, char* testName);

    static std::string ConvertDouble(double number);
};

#endif
