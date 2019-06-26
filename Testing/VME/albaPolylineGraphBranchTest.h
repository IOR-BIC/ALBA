/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPolylineGraphBranchTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAPOLYLINEGRAPHBranchTEST_H__
#define __CPP_UNIT_ALBAPOLYLINEGRAPHBranchTEST_H__

#include "albaTest.h"
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for albaPolylineGraph
//------------------------------------------------------------------------------
class albaPolylineGraphBranchTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaPolylineGraphBranchTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetNumberOfVertices ); 
  CPPUNIT_TEST( TestGetNumberOfEdges ); 
  CPPUNIT_TEST( TestGetSetName );
  CPPUNIT_TEST( TestUnsetName );
  CPPUNIT_TEST( TestGetSetEdgeId );
  CPPUNIT_TEST( TestAddEdgeId );
  CPPUNIT_TEST( TestFindEdgeId );
  CPPUNIT_TEST( TestGetLastEdgeId );
  CPPUNIT_TEST( TestGetSetVertexId );
  CPPUNIT_TEST( TestAddVertexId );
  CPPUNIT_TEST( TestFindVertexId );
  CPPUNIT_TEST( TestGetLastVertexId );
  CPPUNIT_TEST( TestDeleteLastVertex );
  CPPUNIT_TEST( TestGetVerticesIdList );
  CPPUNIT_TEST( TestReverseDirection );
  CPPUNIT_TEST( TestGetSetMappingToOutputPolydata );
  CPPUNIT_TEST(TestSelfCheck );
  CPPUNIT_TEST_SUITE_END();

protected:

  void Test();
  void TestDynamicAllocation();
  void TestGetNumberOfVertices(); 
  void TestGetNumberOfEdges(); 
  void TestGetSetName();
  void TestUnsetName();
  void TestGetSetEdgeId();
  void TestAddEdgeId();
  void TestFindEdgeId();
  //void TestInsertEdgeId();
  void TestGetLastEdgeId();
  void TestGetSetVertexId();
  void TestAddVertexId();
  void TestFindVertexId();
  //void TestInsertVertexId();
  void TestGetLastVertexId();
  void TestDeleteLastVertex();
  void TestGetVerticesIdList();
  void TestReverseDirection();
  void TestGetSetMappingToOutputPolydata();  
  void TestSelfCheck(); 

};

#endif
