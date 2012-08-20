/*=========================================================================

 Program: MAF2Medical
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
  runner.addTest( mafPolylineGraphEdgeTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
