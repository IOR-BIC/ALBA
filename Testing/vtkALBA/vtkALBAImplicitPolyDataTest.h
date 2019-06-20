/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAImplicitPolyDataTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAImplicitPolyDataTest_H__
#define __CPP_UNIT_vtkALBAImplicitPolyDataTest_H__

#include "albaTest.h"

class vtkPolyData;

class vtkALBAImplicitPolyDataTest : public albaTest
{
  public:
  
    CPPUNIT_TEST_SUITE( vtkALBAImplicitPolyDataTest );
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
