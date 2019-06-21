/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPolylineGraphEdgeTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAPOLYLINEGRAPHEdgeTEST_H__
#define __CPP_UNIT_ALBAPOLYLINEGRAPHEdgeTEST_H__

#include "albaTest.h"
#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for albaPolylineGraph
//------------------------------------------------------------------------------
class albaPolylineGraphEdgeTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaPolylineGraphEdgeTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestGetOtherEndVertexId );
  CPPUNIT_TEST( TestGetSetBranchId );
  CPPUNIT_TEST( TestGetSetVertexId );
  CPPUNIT_TEST( TestGetSetVertexIds ) ;
  CPPUNIT_TEST( TestGetSetWeight );
  CPPUNIT_TEST( TestGetVerticesIdList );
  CPPUNIT_TEST( TestGetVerticesIdList );
  CPPUNIT_TEST( TestIsVertexPair );
  CPPUNIT_TEST( TestIsSetVertexPairOrdered );
  CPPUNIT_TEST( TestSetIsDirected );
  CPPUNIT_TEST(TestSelfCheck );
  CPPUNIT_TEST( TestGetSetMappingToOutputPolydata );
  CPPUNIT_TEST_SUITE_END();

protected:

  void Test();
  void TestDynamicAllocation();
  void TestGetSetVertexIds(); 
  void TestGetSetVertexId(); 
  void TestGetVerticesIdList();
  void TestGetOtherEndVertexId() ; 
  void TestGetSetBranchId(); 
  void TestIsVertexPair() ; 
  void TestIsSetVertexPairOrdered(); 
  void TestSetIsDirected() ;
  void TestReverseDirection();
  void TestGetSetWeight();               
  void TestSelfCheck(); 
  void TestGetSetMappingToOutputPolydata();  
};

#endif
