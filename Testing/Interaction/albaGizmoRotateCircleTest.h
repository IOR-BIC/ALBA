/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateCircleTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoRotateCircleTest_H__
#define __CPP_UNIT_albaGizmoRotateCircleTest_H__

#include "albaTest.h"
#include "albaGizmoRotateCircle.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class albaGizmoRotateCircleTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoRotateCircleTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetGetAxis );
  CPPUNIT_TEST( TestSetRadius );
  /** LEAKS!!! for the well known albaMatrix 
  ref count problem (ie not supporting ref count)*/
  CPPUNIT_TEST( TestSetRefSysMatrix );
  CPPUNIT_TEST( TestSetListener);
  CPPUNIT_TEST( TestSetInput );
  CPPUNIT_TEST( TestSetGetAbsPose );
  CPPUNIT_TEST( TestSetGetIsActive );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();

    void RenderGizmo( albaGizmoRotateCircle * gizmoRotateCircle);
    void TestSetGetAxis();
    void TestSetRadius();
    void TestSetGetCylinderLength();
    /** LEAKS!!! needs more investigation */
    void TestSetRefSysMatrix();
    void TestSetGetIsActive();
    void TestSetConstraintModality();
    void TestSetListener();
    void TestSetInput();
    void TestShow();
    void TestSetGetAbsPose();
    void TestSetStep();

	  void CreateTestData();

    albaVMESurface *m_GizmoInputSurface;
    albaVMERoot *m_Root;

    // TODO: REFACTOR THIS 
    // move somewhere else... there should be a common class for testing facilities
    void RenderData(vtkDataSet *data);

    void CreateRenderStuff();     
};

#endif
