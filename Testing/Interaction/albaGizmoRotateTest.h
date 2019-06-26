/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoRotateTest_H__
#define __CPP_UNIT_albaGizmoRotateTest_H__

#include "albaTest.h"
#include "albaGizmoRotate.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class albaGizmoRotateTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoRotateTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetGetListener);
  CPPUNIT_TEST( TestSetGetInput );
  CPPUNIT_TEST( TestSetGetAbsPose );
  CPPUNIT_TEST( TestSetGetRefSys );
  CPPUNIT_TEST( TestSetGetCircleFanRadius );
  CPPUNIT_TEST(TestGetInteractor);
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetGetListener();
    void TestSetGetInput();
    void TestShow();
    void TestSetGetAbsPose();
    void TestSetGetRefSys();
    void TestSetGetCircleFanRadius();
    void TestGetInteractor();
	  void CreateTestData();
   
    albaVMESurface *m_GizmoInputSurface;
    albaVMERoot *m_Root;
    
};

#endif
