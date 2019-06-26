/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAClipSurfaceBoundingBoxTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAClipSurfaceBoundingBoxTEST_H__
#define __CPP_UNIT_vtkALBAClipSurfaceBoundingBoxTEST_H__

#include "albaTest.h"

//----------------------------------------------------------
// Forward Classes:
//----------------------------------------------------------
class vtkSphereSource;
class vtkPlaneSource;
class vtkActorCollection;
class vtkRenderWindow;

/**
  Class Name: vtkALBAClipSurfaceBoundingBoxTest.
  Test class for vtkALBAClipSurfaceBoundingBox.
*/
class vtkALBAClipSurfaceBoundingBoxTest : public albaTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    /** Start Test Suite */
    CPPUNIT_TEST_SUITE( vtkALBAClipSurfaceBoundingBoxTest );
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
    /** Test execution with parametrized clip inside*/
    void TestExecution(int clipInside);
    /** Convert an int to std::string */
    static std::string ConvertInt(int number);

    int m_TestNumber;
};

#endif
