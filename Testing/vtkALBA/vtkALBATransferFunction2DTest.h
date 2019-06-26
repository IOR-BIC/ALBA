/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATransferFunction2DTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBATransferFunction2DTest_H__
#define __CPP_UNIT_vtkALBATransferFunction2DTest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;

//------------------------------------------------------------------------------
// Test class for vtkALBATransferFunction2DTest
//------------------------------------------------------------------------------
class vtkALBATransferFunction2DTest : public albaTest
{
  public:

    CPPUNIT_TEST_SUITE( vtkALBATransferFunction2DTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TetsInitialize );
    CPPUNIT_TEST( TestDeepCopy );
    CPPUNIT_TEST( TestShallowCopy );    
    CPPUNIT_TEST( TestGetDataObjectType );
    CPPUNIT_TEST( TestGetNumberOfWidgets );
    CPPUNIT_TEST( TestAddRemoveWidget );
    CPPUNIT_TEST( TestSetGetWidget );
    CPPUNIT_TEST( TestSetGetWidgetName );
    CPPUNIT_TEST( TestSetGetWidgetVisibility );
    CPPUNIT_TEST( TestSetGetWidgetOpacity );
    CPPUNIT_TEST( TestSetGetWidgetColor );
    CPPUNIT_TEST( TestSetGetWidgetDiffuse );
    CPPUNIT_TEST( TestSetGetWidgetValueRatio );
    CPPUNIT_TEST( TestSetGetWidgetValueRange );
    CPPUNIT_TEST( TestSetGetWidgetGradientRange );
    CPPUNIT_TEST( TestSetGetWidgetGradientInterpolation );
    CPPUNIT_TEST( TestCheckWidget );
    CPPUNIT_TEST( TestGetRange );
    CPPUNIT_TEST( TestGetGradientRange );
    CPPUNIT_TEST( TestGetValue );
    CPPUNIT_TEST( TestGetTable );
    CPPUNIT_TEST( TestSaveToStringLoadFromString );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestDynamicAllocation();
    void TetsInitialize();
    void TestDeepCopy();
    void TestShallowCopy();    
    void TestGetDataObjectType();
    void TestGetNumberOfWidgets();
    void TestAddRemoveWidget();
    void TestSetGetWidget();
    void TestSetGetWidgetName();
    void TestSetGetWidgetVisibility();
    void TestSetGetWidgetOpacity();
    void TestSetGetWidgetColor();
    void TestSetGetWidgetDiffuse();
    void TestSetGetWidgetValueRatio();
    void TestSetGetWidgetValueRange();
    void TestSetGetWidgetGradientRange();
    void TestSetGetWidgetGradientInterpolation();
    void TestCheckWidget();
    void TestGetRange();
    void TestGetGradientRange();
    void TestGetValue();
    void TestGetTable();
    void TestSaveToStringLoadFromString();
};

#endif
