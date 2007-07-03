/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPolylineGraphTest.h,v $
Language:  C++
Date:      $Date: 2007-07-03 15:38:01 $
Version:   $Revision: 1.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __CPP_UNIT_MAFPOLYLINEGRAPHTEST_H__
#define __CPP_UNIT_MAFPOLYLINEGRAPHTEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for mafPolylineGraph
//------------------------------------------------------------------------------
class mafPolylineGraphTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( mafPolylineGraphTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestCopyFromPolydata );
    CPPUNIT_TEST( TestCopyToPolydata );
    CPPUNIT_TEST( TestMapping ) ;
    CPPUNIT_TEST( TestAddNewVertex1 );
    CPPUNIT_TEST( TestAddNewVertex2 );
    CPPUNIT_TEST( TestAddNewEdge );
    CPPUNIT_TEST( TestAddNewBranch );
    CPPUNIT_TEST( TestSplitBranchAtEdge );
    CPPUNIT_TEST( TestSplitBranchAtVertex );
    CPPUNIT_TEST( TestDeleteEdge );
    CPPUNIT_TEST( TestDeleteVertex );
    CPPUNIT_TEST( TestDeleteBranch );
    CPPUNIT_TEST( TestAddNewVertexToBranch);
    CPPUNIT_TEST( TestAddExistingVertexToBranch);
    CPPUNIT_TEST( TestAddExistingEdgeToBranch);
    CPPUNIT_TEST( TestBranchName );
    CPPUNIT_TEST( TestReverseBranch );
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestCopyFromPolydata();
    void TestCopyToPolydata();
    void TestMapping() ;
    void TestAddNewVertex1();
    void TestAddNewVertex2();
    void TestAddNewEdge() ;
    void TestAddNewBranch() ;
    void TestSplitBranchAtEdge() ;
    void TestSplitBranchAtVertex() ;
    void TestDeleteEdge() ;
    void TestDeleteVertex() ;
    void TestDeleteBranch() ;
    void TestAddNewVertexToBranch() ;
    void TestAddExistingVertexToBranch() ;
    void TestAddExistingEdgeToBranch() ;
    void TestBranchName() ;
    void TestReverseBranch() ;
		void CreateExamplePolydata();
    vtkPolyData *m_Polydata ;
};


int 
main( int argc, char* argv[] )
{

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( mafPolylineGraphTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
