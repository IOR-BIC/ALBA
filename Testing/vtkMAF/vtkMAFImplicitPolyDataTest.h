/*=========================================================================

 Program: MAF2
 Module: vtkMAFImplicitPolyDataTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFImplicitPolyDataTest_H__
#define __CPP_UNIT_vtkMAFImplicitPolyDataTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

class vtkPolyData;

class vtkMAFImplicitPolyDataTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFImplicitPolyDataTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestConstructorDestructor );
    CPPUNIT_TEST( TestEvaluateGradient );
    CPPUNIT_TEST( TestEvaluateFunction );
    CPPUNIT_TEST( TestSetInput );
    CPPUNIT_TEST( TestSetGetNoValue );
    CPPUNIT_TEST( TestSetGetNoGradient );
    CPPUNIT_TEST( TestClipSphere1PolydataWithSphere2PolydataUsedAsImplicitFunction );
    CPPUNIT_TEST( TestGenerateOffsetSurfaceFromPolydata );
    CPPUNIT_TEST( TestUnionBetweenTwoPolydata );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void TestFixture();
    void TestConstructorDestructor();
    void TestEvaluateGradient();
  	void TestEvaluateFunction();
    void TestSetInput();
    void TestSetGetNoValue();
    void TestSetGetNoGradient();

    /** several test/use cases to showcase filter features */
    void TestClipSphere1PolydataWithSphere2PolydataUsedAsImplicitFunction();
    void TestGenerateOffsetSurfaceFromPolydata();
    void TestUnionBetweenTwoPolydata();

    /** create interactive render window for polydata */
    void RenderData(vtkPolyData *data);
};

#endif
