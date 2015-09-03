/*=========================================================================

 Program: MAF2
 Module: vtkMAFTextActorMeterTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFTextActorMetertest_H__
#define __CPP_UNIT_vtkMAFTextActorMetertest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class vtkMAFTextActorMeter;

//------------------------------------------------------------------------------
// Test class for vtkMAFTextActorMeter
//------------------------------------------------------------------------------
class vtkMAFTextActorMeterTest : public mafTest
{
  public:
    enum ID_TEST_LIST
    {
      ID_TEXT_TEST_POS1 = 0,
      ID_TEXT_TEST_POS2,
      ID_TEXT_COLOR_TEST,
    };

    CPPUNIT_TEST_SUITE( vtkMAFTextActorMeterTest );
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
    void SetText(vtkMAFTextActorMeter *actor, const char *text, double position[3]);
    void RenderData(vtkActor2D *actor );

    void CompareImages(vtkRenderWindow * renwin);

    static std::string ConvertInt(int number);
    static std::string ConvertDouble(double number);
    
    int m_TestNumber;
};

#endif
