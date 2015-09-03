/*=========================================================================

 Program: MAF2
 Module: mafGizmoScaleTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoScaleTest_H__
#define __CPP_UNIT_mafGizmoScaleTest_H__

#include "mafTest.h"
#include "mafGizmoScale.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class mafGizmoScaleTest : public mafTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( mafGizmoScaleTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestConstructor );
    CPPUNIT_TEST( TestSetGetListener);
    CPPUNIT_TEST( TestSetGetInput );
    CPPUNIT_TEST( TestSetGetAbsPose );
    CPPUNIT_TEST( TestSetGetRefSys );
    CPPUNIT_TEST( TestShow );
    CPPUNIT_TEST_SUITE_END();

  protected:
    
    void TestFixture();
    void TestConstructor();
    void TestSetGetListener();
    void TestSetGetInput();
    void TestSetGetAbsPose();
    void TestSetGetRefSys();
    void TestShow();

	  void CreateTestData();
   
    mafVMESurface *m_GizmoInputSurface;
    mafVMERoot *m_Root;
    
};

#endif
