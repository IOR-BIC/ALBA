/*=========================================================================

 Program: MAF2Medical
 Module: medGeometryEditorPolylineGraphTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_medGeometryEditorPolylineGraphTest_H__
#define __CPP_UNIT_medGeometryEditorPolylineGraphTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

//------------------------------------------------------------------------------
// Test class for mafPolylineGraph
//------------------------------------------------------------------------------
class medGeometryEditorPolylineGraphTest : public CPPUNIT_NS::TestFixture
{
public:

	// CPPUNIT fixture: executed before each test
	void setUp();

	// CPPUNIT fixture: executed after each test
	void tearDown();

	CPPUNIT_TEST_SUITE( medGeometryEditorPolylineGraphTest );
	CPPUNIT_TEST( TestFixture );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestAddNewVertex1 );
	CPPUNIT_TEST( TestAddNewVertex2 );
	CPPUNIT_TEST( TestAddBranch );
	CPPUNIT_TEST( TestSelectPoint );
	CPPUNIT_TEST( TestDeletePoint );
	CPPUNIT_TEST( TestSelectBranch );
	CPPUNIT_TEST( TestInsertPoint );
	CPPUNIT_TEST_SUITE_END();

protected:

	void TestFixture();
	void TestDynamicAllocation();
	void TestStaticAllocation();
	void TestAddNewVertex1();
	void TestAddNewVertex2();
	void TestAddBranch();
	void TestSelectPoint();
	void TestDeletePoint();
	void TestSelectBranch();
	void TestInsertPoint();

	void CreateExampleGraph();

	vtkPolyData *m_Graph;
	int m_NumberOfEdgesGraph;
	int m_NumberOfBranchesGraph;
	int m_NumberOfPointsGraph;
	int m_NumberOfLinesGraph;
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
	runner.addTest( medGeometryEditorPolylineGraphTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
