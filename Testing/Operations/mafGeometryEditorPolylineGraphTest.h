/*=========================================================================

 Program: MAF2
 Module: mafGeometryEditorPolylineGraphTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGeometryEditorPolylineGraphTest_H__
#define __CPP_UNIT_mafGeometryEditorPolylineGraphTest_H__

#include "mafTest.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

//------------------------------------------------------------------------------
// Test class for mafPolylineGraph
//------------------------------------------------------------------------------
class mafGeometryEditorPolylineGraphTest : public mafTest
{
public:

	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

	CPPUNIT_TEST_SUITE( mafGeometryEditorPolylineGraphTest );
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

#endif
