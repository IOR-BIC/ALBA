/*=========================================================================

 Program: MAF2
 Module: vtkMAFTextOrientatorTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFTextOrientatortest_H__
#define __CPP_UNIT_vtkMAFTextOrientatortest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkMAFTextOrientator;
class vtkActor2D;
//------------------------------------------------------------------------------
// Test class for vtkMAFTextOrientator
//------------------------------------------------------------------------------
class vtkMAFTextOrientatorTest : public mafTest
{
  public:
		// Executed before each test
		void BeforeTest();

		// Executed after each test
		void AfterTest();

    CPPUNIT_TEST_SUITE( vtkMAFTextOrientatorTest );
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
    void SetText(vtkMAFTextOrientator *actor);
    void RenderData(vtkActor2D *actor, char* testName);
};

#endif
