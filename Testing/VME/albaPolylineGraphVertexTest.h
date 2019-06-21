/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPolylineGraphVertexTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_ALBAPOLYLINEGRAPHVERTEXTEST_H__
#define __CPP_UNIT_ALBAPOLYLINEGRAPHVERTEXTEST_H__

#include "albaTest.h"

#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for albaPolylineGraph
//------------------------------------------------------------------------------
class albaPolylineGraphVertexTest : public albaTest
{
public:

  CPPUNIT_TEST_SUITE( albaPolylineGraphVertexTest );
  CPPUNIT_TEST( TestDynamicAllocation );
  CPPUNIT_TEST( TestAddEdgeId );
  CPPUNIT_TEST( TestGetDegree );
  CPPUNIT_TEST( TestSetGetEdgeId ) ;
  CPPUNIT_TEST( TestFindEdgeId );
  CPPUNIT_TEST( TestDeleteEdgeId );
  CPPUNIT_TEST( TestSetGetVertexId );
  CPPUNIT_TEST( TestAddVertexId );
  CPPUNIT_TEST( TestFindVertexId );
  CPPUNIT_TEST( TestDeleteVertexId );
  CPPUNIT_TEST( TestGetVerticesIdList );
  CPPUNIT_TEST( TestSetGetCoords );
  CPPUNIT_TEST(TestSelfCheck );
  CPPUNIT_TEST(TestClear );
  CPPUNIT_TEST_SUITE_END();

protected:

  void Test();
  void TestDynamicAllocation();
  void TestGetDegree(); 
  void TestSetGetEdgeId(); 
  void TestAddEdgeId() ; 
  void TestFindEdgeId(); 
  void TestDeleteEdgeId() ; 
  void TestSetGetVertexId(); 
  void TestAddVertexId() ; 
  void TestFindVertexId(); 
  void TestDeleteVertexId() ;   
  void TestGetVerticesIdList(); 
  void TestSetGetCoords();        
  void TestClear() ;         
  void TestSelfCheck(); 
};

#endif
