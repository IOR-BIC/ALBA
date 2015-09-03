/*=========================================================================

 Program: MAF2
 Module: mafAxesTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafAxesTest_H__
#define __CPP_UNIT_mafAxesTest_H__

#include "mafTest.h"
#include "vtkRenderer.h"
#include "mafVMESurface.h"

class mafAxesTest : public mafTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( mafAxesTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestConstructorDestructor );
    CPPUNIT_TEST( TestSetVisibility);
    CPPUNIT_TEST( TestSetPose );
    CPPUNIT_TEST( TestRenderData );
    CPPUNIT_TEST_SUITE_END();

  private:

    void TestConstructorDestructor();
    void TestFixture();
    void TestRenderData();
  	void TestSetVisibility();
    void TestSetPose();

    void RenderData(vtkDataSet *data);
    
    mafVMESurface *m_VMESurfaceSphere;
    vtkRenderer *m_Renderer;
};




#endif
