/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBALineStripSourceTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBALineStripSourceTest_H__
#define __CPP_UNIT_vtkALBALineStripSourceTest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class vtkALBALineStripSource;

//------------------------------------------------------------------------------
// Test class for vtkALBALineStripSourceTest
//------------------------------------------------------------------------------
class vtkALBALineStripSourceTest : public albaTest
{
	CPPUNIT_TEST_SUITE(vtkALBALineStripSourceTest);
	CPPUNIT_TEST(TestDynamicAllocation);
	CPPUNIT_TEST(TestPoints);
	CPPUNIT_TEST(TestClassName);
	CPPUNIT_TEST(TestCells);
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
	void TestPoints();
	void TestClassName();
	void TestCells();
};

#endif
