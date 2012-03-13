/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEGizmoTest.h,v $
Language:  C++
Date:      $Date: 2006-10-23 13:45:33 $
Version:   $Revision: 1.5 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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


int
main( int argc, char* argv[] )
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that collects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafVMEGizmoTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}
#endif
