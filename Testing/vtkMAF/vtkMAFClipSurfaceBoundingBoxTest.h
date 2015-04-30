/*=========================================================================

 Program: MAF2
 Module: vtkMAFClipSurfaceBoundingBoxTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFClipSurfaceBoundingBoxTEST_H__
#define __CPP_UNIT_vtkMAFClipSurfaceBoundingBoxTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

//----------------------------------------------------------
// Forward Classes:
//----------------------------------------------------------
class vtkSphereSource;
class vtkPlaneSource;
class vtkActorCollection;
class vtkRenderWindow;

/**
  Class Name: vtkMAFClipSurfaceBoundingBoxTest.
  Test class for vtkMAFClipSurfaceBoundingBox.
*/
class vtkMAFClipSurfaceBoundingBoxTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    /** Start Test Suite */
    CPPUNIT_TEST_SUITE( vtkMAFClipSurfaceBoundingBoxTest );
    /** Test for dynamic allocation */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test for set and get correct mask */
    CPPUNIT_TEST( TestSetGetMask );
    /** Test set and get correct clip indide flag */
    CPPUNIT_TEST( TestSetGetClipInside );
    /** Test execution with clip inside off*/
    CPPUNIT_TEST( TestExecutionClipInsideOff );
    /** Test execution with clip inside on*/
    CPPUNIT_TEST( TestExecutionClipInsideOn );
    /** End Test Suite */
    CPPUNIT_TEST_SUITE_END();

  protected:
 
    /** Allocate and Deallocate filter */
    void TestDynamicAllocation();
    /** Test for set and get correct mask */
    void TestSetGetMask();
    /** Test set and get correct clip indide flag */
    void TestSetGetClipInside();
    /** Test execution with clip inside off*/
    void TestExecutionClipInsideOff();
    /** Test execution with clip inside on*/
    void TestExecutionClipInsideOn();

  private:
    vtkSphereSource *m_SphereInput;
    vtkPlaneSource *m_PlaneMask;

    /** Render data in vtk render window.*/
    void RenderData(vtkActorCollection *actor );
    /** After saving images from render window, compare them in order to find difference between test sessions. */
    void CompareImages(vtkRenderWindow * renwin);
    /** Test execution with parametrized clip inside*/
    void TestExecution(int clipInside);
    /** Convert an int to std::string */
    static std::string ConvertInt(int number);

    int m_TestNumber;

};

#endif
