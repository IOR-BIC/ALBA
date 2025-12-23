/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoROITest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoROITest_H__
#define __CPP_UNIT_albaGizmoROITest_H__

#include "albaTest.h"
#include "albaGizmoROI.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class albaGizmoROITest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoROITest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetListener);
  CPPUNIT_TEST( TestSetInput );
  CPPUNIT_TEST( TestSetGetBounds );
  CPPUNIT_TEST( TestShow );
  CPPUNIT_TEST( TestShowHandles);
  CPPUNIT_TEST( TestShowShadingPlane );
  CPPUNIT_TEST( TestShowROI );
  CPPUNIT_TEST( TestHighlight );
  CPPUNIT_TEST( TestHighlightOff );
  CPPUNIT_TEST( TestSetConstrainRefSys );
  CPPUNIT_TEST( TestEnableMinimumHandleSize );
  CPPUNIT_TEST( TestSetMinimumHandleSize );
  CPPUNIT_TEST( TestReset );
  CPPUNIT_TEST_SUITE_END();
  
  protected:
    
    void TestFixture();

    void TestConstructor();
    void TestSetListener();
    void TestSetInput();
    void TestShow();
    void TestShowHandles();
    void TestShowShadingPlane();
    void TestShowROI();
    void TestSetGetBounds();
    void TestHighlight();
    void TestHighlightOff();
    void TestSetConstrainRefSys();
    void TestEnableMinimumHandleSize();
    void TestSetMinimumHandleSize();
    void TestReset();

    void CreateTestData();

    void RenderGizmo( albaGizmoROI *gizmoROI );

    albaVMESurface *m_GizmoInputSurface;
    albaVMERoot *m_Root;

};

#endif
