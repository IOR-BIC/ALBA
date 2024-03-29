/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPolylineGraphTest
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAPOLYLINEGRAPHTEST_H__
#define __CPP_UNIT_ALBAPOLYLINEGRAPHTEST_H__

#include "albaTest.h"
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for albaPolylineGraph
//------------------------------------------------------------------------------
class albaPolylineGraphTest : public albaTest
{
  public:
  
    // Executed before each test
    void BeforeTest();

    // Executed after each test
    void AfterTest();

    CPPUNIT_TEST_SUITE( albaPolylineGraphTest );
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
