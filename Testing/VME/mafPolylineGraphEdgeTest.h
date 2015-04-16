/*=========================================================================

 Program: MAF2
 Module: mafPolylineGraphEdgeTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_MAFPOLYLINEGRAPHEdgeTEST_H__
#define __CPP_UNIT_MAFPOLYLINEGRAPHEdgeTEST_H__

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
class mafPolylineGraphEdgeTest : public CPPUNIT_NS::TestFixture
{
public:

  // CPPUNIT fixture: executed before each test
  void setUp();

  // CPPUNIT fixture: executed after each test
  void tearDown();

  CPPUNIT_TEST_SUITE( mafPolylineGraphEdgeTest );
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
