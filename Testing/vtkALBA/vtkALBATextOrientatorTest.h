/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBATextOrientatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBATextOrientatortest_H__
#define __CPP_UNIT_vtkALBATextOrientatortest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkALBATextOrientator;
class vtkActor2D;
//------------------------------------------------------------------------------
// Test class for vtkALBATextOrientator
//------------------------------------------------------------------------------
class vtkALBATextOrientatorTest : public albaTest
{
  public:
		// Executed before each test
		void BeforeTest();

		// Executed after each test
		void AfterTest();

    CPPUNIT_TEST_SUITE( vtkALBATextOrientatorTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestDynamicAllocation );
    CPPUNIT_TEST( TestText );
    CPPUNIT_TEST( TestSingleActorVisibility );
    CPPUNIT_TEST( TestTextColor );
    CPPUNIT_TEST( TestBackgroundColor );
    CPPUNIT_TEST( TestBackgroundVisibility );
    CPPUNIT_TEST( TestScale );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestDynamicAllocation();
    void TestText();
    void TestSingleActorVisibility();
    void TestTextColor();
    void TestBackgroundColor();
    void TestBackgroundVisibility();
    void TestScale();

    //accessories
    void SetText(vtkALBATextOrientator *actor);
    void RenderData(vtkActor2D *actor, char* testName);
};

#endif
