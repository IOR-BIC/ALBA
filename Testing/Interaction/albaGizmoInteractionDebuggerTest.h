/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoInteractionDebuggerTest
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaGizmoInteractionDebuggerTest_H__
#define __CPP_UNIT_albaGizmoInteractionDebuggerTest_H__

#include "albaTest.h"
#include "albaVMEPolylineGraph.h"
#include "albaVMESurface.h"
#include "albaVMERoot.h"

class albaGizmoInteractionDebuggerTest : public albaTest
{
public:

	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

	CPPUNIT_TEST_SUITE( albaGizmoInteractionDebuggerTest );
	CPPUNIT_TEST( TestFixture );
	CPPUNIT_TEST( TestConstructorDestructor );
	CPPUNIT_TEST_SUITE_END();

protected:

	void TestFixture();
	void TestConstructorDestructor();
	
	albaVMEPolylineGraph *m_PolylineGraph;
	albaVMESurface *m_GizmoInputSurface;
	albaVMERoot *m_Root;

};

#endif
