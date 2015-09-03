/*=========================================================================

 Program: MAF2
 Module: mafGizmoTranslateAxisTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoTranslateAxisTest_H__
#define __CPP_UNIT_mafGizmoTranslateAxisTest_H__

#include "mafTest.h"
#include "mafGizmoTranslateAxis.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class mafGizmoTranslateAxisTest : public mafTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGizmoTranslateAxisTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetGetAxis );
  CPPUNIT_TEST( TestSetGetConeLength );
  CPPUNIT_TEST( TestSetGetCylinderLength );
  /** LEAKS!!! needs more investigation */
  CPPUNIT_TEST( TestSetRefSysMatrix );
  CPPUNIT_TEST( TestSetConstraintModality );
  CPPUNIT_TEST( TestSetListener);
  CPPUNIT_TEST( TestSetInput );
  CPPUNIT_TEST( TestSetGetAbsPose );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();

    void RenderGizmo( mafGizmoTranslateAxis * gizmoTranslateAxis );
    void TestSetGetAxis();
    void TestSetGetConeLength();
    void TestSetGetCylinderLength();
    /** LEAKS!!! needs more investigation */
    void TestSetRefSysMatrix();
    void TestSetConstraintModality();
    void TestSetListener();
    void TestSetInput();
    void TestShow();
    void TestSetGetAbsPose();
    void TestSetStep();

	  void CreateTestData();

    mafVMESurface *m_GizmoInputSurface;
    mafVMERoot *m_Root;

    // TODO: REFACTOR THIS 
    // move somewhere else... there should be a common class for testing facilities
    void RenderData(vtkDataSet *data);

    void CreateRenderStuff(); 
      
    vtkRenderer *m_Renderer;
    vtkRenderWindow *m_RenderWindow;
    vtkRenderWindowInteractor *m_RenderWindowInteractor;    
    
};

#endif
