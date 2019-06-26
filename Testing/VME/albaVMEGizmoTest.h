/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGizmoTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVMEGizmoTest_H__
#define __CPP_UNIT_albaVMEGizmoTest_H__

#include "albaTest.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEGeneric.h"

/** Test for albaVMEGizmo; the tested gizmo is visualized as a sphere. Use this suite to trace memory problems */
class albaVMEGizmoTest : public albaTest
{
  public: 

    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( albaVMEGizmoTest );
    CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
    CPPUNIT_TEST( TestGizmoCreateAndDestroy );
    CPPUNIT_TEST( TestGizmoVisualizatioAlone );
    CPPUNIT_TEST( TestGizmoVisualizationInVMETree ); // watch for the sphere...
    CPPUNIT_TEST( TestGizmoVisibleToTraverse ); // the sphere should not show...
    CPPUNIT_TEST_SUITE_END();

 private:
    
    void TestFixture();
    void TestGizmoCreateAndDestroy();
    void TestGizmoVisualizatioAlone();
    void TestGizmoVisualizationInVMETree();
    void TestGizmoVisibleToTraverse();
    
    // helper methods 
    void CreateVMETestTree();
    int PlayTree(albaVMERoot *root, bool ignoreVisibleToTraverse);

    // helper variables 
    albaVMERoot *m_VmeRoot;
    albaVMEGeneric *m_FirstChild;

};

#endif
