/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAxesTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaAxesTest_H__
#define __CPP_UNIT_albaAxesTest_H__

#include "albaTest.h"
#include "vtkRenderer.h"
#include "albaVMESurface.h"

class albaAxesTest : public albaTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( albaAxesTest );
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
    
    albaVMESurface *m_VMESurfaceSphere;
    vtkRenderer *m_Renderer;
};




#endif
