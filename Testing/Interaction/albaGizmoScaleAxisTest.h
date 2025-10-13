/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoScaleAxisTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoScaleAxisTest_H__
#define __CPP_UNIT_albaGizmoScaleAxisTest_H__

#include "albaTest.h"
#include "albaGizmoScaleAxis.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class albaGizmoScaleAxisTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoScaleAxisTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetGetAxis );
  CPPUNIT_TEST( TestSetGetCubeLength );
  CPPUNIT_TEST( TestSetGetCylinderLength );
  CPPUNIT_TEST( TestSetRefSysMatrix );
  CPPUNIT_TEST( TestSetListener );
  CPPUNIT_TEST( TestSetGetInput );
  CPPUNIT_TEST( TestShow );
  CPPUNIT_TEST( TestSetGetAbsPose );
  CPPUNIT_TEST( TestHighlight );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetGetAxis();
    void TestSetGetCubeLength();
    void TestSetGetCylinderLength();
    void TestSetRefSysMatrix();
    void TestSetListener();
    void TestSetGetInput();
    void TestShow();
    void TestSetGetAbsPose();
    void TestHighlight();


    void RenderGizmo( albaGizmoScaleAxis * gizmoScaleAxis );

	  void CreateTestData();

    albaVMESurface *m_GizmoInputSurface;
    albaVMERoot *m_Root;

    // TODO: REFACTOR THIS 
    // move somewhere else... there should be a common class for testing facilities
    void RenderData(vtkDataSet *data);

    void CreateRenderStuff();     
};

#endif
