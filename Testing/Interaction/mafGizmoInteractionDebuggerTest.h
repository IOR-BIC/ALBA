/*=========================================================================

 Program: MAF2
 Module: mafGizmoInteractionDebuggerTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafGizmoInteractionDebuggerTest_H__
#define __CPP_UNIT_mafGizmoInteractionDebuggerTest_H__

#include "mafTest.h"
#include "mafVMEPolylineGraph.h"
#include "mafVMESurface.h"
#include "mafVMERoot.h"

class mafGizmoInteractionDebuggerTest : public mafTest
{
public:

	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

	CPPUNIT_TEST_SUITE( mafGizmoInteractionDebuggerTest );
	CPPUNIT_TEST( TestFixture );
	CPPUNIT_TEST( TestConstructorDestructor );
	CPPUNIT_TEST_SUITE_END();

protected:

	void TestFixture();
	void TestConstructorDestructor();
	
	mafVMEPolylineGraph *m_PolylineGraph;
	mafVMESurface *m_GizmoInputSurface;
	mafVMERoot *m_Root;

};

#endif
