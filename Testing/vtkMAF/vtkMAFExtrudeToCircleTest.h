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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for vtkMAFExtrudeToCircle
//------------------------------------------------------------------------------
class vtkMAFExtrudeToCircleTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

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
