/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExtrusionHolesTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef CPP_UNIT_albaOpExtrusionHolesTEST_H
#define CPP_UNIT_albaOpExtrusionHolesTEST_H

#include "albaTest.h"

class albaOpExtrusionHolesTest : public albaTest
{
	CPPUNIT_TEST_SUITE( albaOpExtrusionHolesTest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestSetGetExtrusionFactor );
	CPPUNIT_TEST( TestExtractFreeEdge );
	CPPUNIT_TEST( TestExtrude );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
	void TestStaticAllocation();
	void TestSetGetExtrusionFactor();
	void TestExtractFreeEdge();
	void TestExtrude();
};

#endif
