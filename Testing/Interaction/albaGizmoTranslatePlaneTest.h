/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoTranslatePlaneTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoTranslatePlaneTest_H__
#define __CPP_UNIT_albaGizmoTranslatePlaneTest_H__

#include "albaTest.h"
#include "albaGizmoTranslatePlane.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class albaGizmoTranslatePlaneTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoTranslatePlaneTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  /** LEAKS!!! needs more investigation */
  CPPUNIT_TEST( TestSetConstrainRefSys );
  CPPUNIT_TEST( TestSetConstraintModality );
  CPPUNIT_TEST( TestSetListener);
  CPPUNIT_TEST( TestSetInput );
  CPPUNIT_TEST( TestSetGetAbsPose );
  CPPUNIT_TEST( TestSetGetPlane );
  CPPUNIT_TEST( TestSetGetLength );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();

    void RenderGizmo( albaGizmoTranslatePlane* gizmoTranslateAxis );
    void TestSetGetPlane();
    void TestSetGetLength();
    /** LEAKS!!! needs more investigation */
    void TestSetConstrainRefSys();
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
