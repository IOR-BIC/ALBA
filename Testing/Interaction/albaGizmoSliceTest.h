/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoSliceTest
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoSliceTest_H__
#define __CPP_UNIT_albaGizmoSliceTest_H__

#include "albaTest.h"
#include "albaGizmoSlice.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class albaGizmoSliceTest : public albaTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaGizmoSliceTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetGetAbsPose );
  CPPUNIT_TEST( TestSetGetModality );
  CPPUNIT_TEST( TestSetGetMovingModality );
  CPPUNIT_TEST( TestGetGui );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestShow();
    void TestSetGetAbsPose();
    void TestSetGetModality();
    void TestSetGetMovingModality();
    void TestGetGui();
	  void CreateTestData();
   
    albaVMESurface *m_GizmoInputSurface;
    albaVMERoot *m_Root;
    
};

#endif
