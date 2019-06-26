/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFillingHoleCEdgeTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAFillingHoleCEdgeTEST_H__
#define __CPP_UNIT_vtkALBAFillingHoleCEdgeTEST_H__

#include "albaTest.h"

class vtkALBAFillingHoleCEdgeTest : public albaTest
{
	CPPUNIT_TEST_SUITE( vtkALBAFillingHoleCEdgeTest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestSetTriangle );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
	void TestSetTriangle();
};

#endif