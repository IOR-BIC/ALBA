/*=========================================================================

 Program: MAF2
 Module: mafGizmoInterfaceTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoInterfaceTest_H__
#define __CPP_UNIT_mafGizmoInterfaceTest_H__

#include "mafTest.h"
#include "mafGizmoInterface.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class mafGizmoInterfaceTest : public mafTest
{
  public:
  
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafGizmoInterfaceTest );
  CPPUNIT_TEST( TestFixture );
  CPPUNIT_TEST( TestConstructor );
  CPPUNIT_TEST( TestSetGetListener);
  CPPUNIT_TEST( TestSetGetInput );
  CPPUNIT_TEST( TestSetGetAbsPose );
  CPPUNIT_TEST( TestSetGetModality );
  CPPUNIT_TEST( TestGetGui );
  CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetGetListener();
    void TestSetGetInput();
    void TestShow();
    void TestSetGetAbsPose();
    void TestSetGetModality();
    void TestGetGui();
	  void CreateTestData();
   
    mafVMESurface *m_GizmoInputSurface;
    mafVMERoot *m_Root;
    
};

#endif
