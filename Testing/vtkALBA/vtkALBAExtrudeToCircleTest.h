/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAExtrudeToCircleTest
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAExtrudeToCircletest_H__
#define __CPP_UNIT_vtkALBAExtrudeToCircletest_H__

#include "albaTest.h"
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for vtkALBAExtrudeToCircle
//------------------------------------------------------------------------------
class vtkALBAExtrudeToCircleTest : public albaTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( vtkALBAExtrudeToCircleTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestExtrusion );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestExtrusion();

    // Create test polydata
    void CreateTestData() ;

    // render input and output data
    void RenderExtrusion() ;

    vtkPolyData *m_testData ;
    vtkPolyData *m_extrusion ;
};

#endif
