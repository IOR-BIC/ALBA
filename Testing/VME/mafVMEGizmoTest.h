/*=========================================================================

 Program: MAF2
 Module: mafVMEGizmoTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVMEGizmoTest_H__
#define __CPP_UNIT_mafVMEGizmoTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"

/** Test for mafVMEGizmo; the tested gizmo is visualized as a sphere. Use this suite to trace memory problems */
class mafVMEGizmoTest : public CPPUNIT_NS::TestFixture
{
  public: 

    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    // CPPUNIT test suite
    CPPUNIT_TEST_SUITE( mafVMEGizmoTest );
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
    int PlayTree(mafVMERoot *root, bool ignoreVisibleToTraverse);

    // helper variables 
    mafVMERoot *m_VmeRoot;
    mafVMEGeneric *m_FirstChild;

};

#endif
