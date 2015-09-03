/*=========================================================================

 Program: MAF2
 Module: mafPolylineGraphVertexTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFPOLYLINEGRAPHVERTEXTEST_H__
#define __CPP_UNIT_MAFPOLYLINEGRAPHVERTEXTEST_H__

#include "mafTest.h"

#include "vtkPolyData.h"


//------------------------------------------------------------------------------
// Test class for mafPolylineGraph
//------------------------------------------------------------------------------
class mafPolylineGraphVertexTest : public mafTest
{
public:

  CPPUNIT_TEST_SUITE( mafPolylineGraphVertexTest );
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
