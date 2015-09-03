/*=========================================================================

 Program: MAF2
 Module: vtkMAFExtrudeToCircleTest
 Authors: Nigel McFarlane
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFExtrudeToCircletest_H__
#define __CPP_UNIT_vtkMAFExtrudeToCircletest_H__

#include "mafTest.h"
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for vtkMAFExtrudeToCircle
//------------------------------------------------------------------------------
class vtkMAFExtrudeToCircleTest : public mafTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( vtkMAFExtrudeToCircleTest );
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
