/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBACircleSourceTest
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#ifndef __CPP_UNIT_vtkALBACircleSourceTest_H__
#define __CPP_UNIT_vtkALBACircleSourceTest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class vtkALBACircleSource;

//------------------------------------------------------------------------------
// Test class for vtkALBACircleSourceTest
//------------------------------------------------------------------------------
class vtkALBACircleSourceTest : public albaTest
{
	CPPUNIT_TEST_SUITE(vtkALBACircleSourceTest);
	CPPUNIT_TEST(TestDynamicAllocation);
	CPPUNIT_TEST(TestParameters);
	CPPUNIT_TEST(TestClassName);
	CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
	void TestParameters();
	void TestClassName();
};

#endif
