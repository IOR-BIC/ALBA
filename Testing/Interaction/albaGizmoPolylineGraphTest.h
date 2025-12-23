/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPolylineGraphTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoPolylineGraphTest_H__
#define __CPP_UNIT_albaGizmoPolylineGraphTest_H__

#include "albaTest.h"
#include "albaGizmoPolylineGraph.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class albaGizmoPolylineGraphTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoPolylineGraphTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetInput );
  CPPUNIT_TEST( TestSetGizmoLength );
  CPPUNIT_TEST( TestShow );
  CPPUNIT_TEST_SUITE_END();
  
  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetInput();
    void TestSetGizmoLength();
    void TestShow();
    void CreateTestData();

    void RenderGizmo( albaGizmoPolylineGraph *gizmoROI );

    albaVMESurface *m_GizmoInputSurface;
    albaVMERoot *m_Root;
};

#endif
