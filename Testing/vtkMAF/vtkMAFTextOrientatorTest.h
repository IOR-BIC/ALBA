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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class vtkMAFTextOrientator;
class vtkActor2D;
//------------------------------------------------------------------------------
// Test class for vtkMAFTextOrientator
//------------------------------------------------------------------------------
class vtkMAFTextOrientatorTest : public CPPUNIT_NS::TestFixture
{
  public:
    enum ID_TEST_LIST
    {
      ID_TEXT_TEST = 0,
      ID_SINGLE_VISIBILITY_TEST,
      ID_TEXT_COLOR_TEST,
      ID_BACKGROUND_COLOR_TEST,
      ID_BACKGROUND_VISIBILITY_TEST,
      ID_SCALE_TEST
    };

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

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
    void RenderData(vtkActor2D *actor );

    void CompareImages(vtkRenderWindow * renwin);

    static std::string ConvertInt(int number);
    
    int m_TestNumber;
};

#endif
