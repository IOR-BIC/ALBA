/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFillingHoleCTriangleTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAFillingHoleCTriangleTEST_H__
#define __CPP_UNIT_vtkALBAFillingHoleCTriangleTEST_H__

#include "albaTest.h"

class vtkALBAFillingHoleCTriangleTest : public albaTest
{
	CPPUNIT_TEST_SUITE( vtkALBAFillingHoleCTriangleTest );
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestSetEdge );
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
	void TestSetEdge();
};

#endif