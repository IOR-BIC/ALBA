/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateFanTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoRotateFanTest_H__
#define __CPP_UNIT_albaGizmoRotateFanTest_H__

#include "albaTest.h"
#include "albaGizmoRotateFan.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class albaGizmoRotateFanTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoRotateFanTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetListener);
  CPPUNIT_TEST( TestSetInput );
  CPPUNIT_TEST( TestSetGetAbsPose ); 
  CPPUNIT_TEST( TestSetGetAxis );
  CPPUNIT_TEST( TestShow );
  CPPUNIT_TEST( TestSetGetRadius );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetListener();
    void TestSetInput();
    void TestShow();
    void TestSetGetAbsPose();
    void TestSetGetAxis();
    void TestSetGetRadius();
    void CreateTestData();
  
    void RenderGizmo( albaGizmoRotateFan *gizmoTranslateAxis );

    albaVMESurface *m_GizmoInputSurface;
    albaVMERoot *m_Root;
};

#endif
