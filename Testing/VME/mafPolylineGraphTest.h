/*=========================================================================

 Program: MAF2
 Module: mafPolylineGraphTest
 Authors: Nigel McFarlane
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
    CPPUNIT_TEST( TestGetBranchIntervalLength );
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
    void TestGetBranchIntervalLength() ;
		void CreateExamplePolydata();
    vtkPolyData *m_Polydata ;
};

#endif
