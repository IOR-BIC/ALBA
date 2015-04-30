/*=========================================================================

 Program: MAF2
 Module: mafGizmoScaleAxisTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoScaleAxisTest_H__
#define __CPP_UNIT_mafGizmoScaleAxisTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafGizmoScaleAxis.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

class mafGizmoScaleAxisTest : public CPPUNIT_NS::TestFixture
{
  public:
  
  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafGizmoScaleAxisTest );
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


    void RenderGizmo( mafGizmoScaleAxis * gizmoScaleAxis );

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
