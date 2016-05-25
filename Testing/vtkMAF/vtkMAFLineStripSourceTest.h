/*=========================================================================

 Program: MAF2
 Module: vtkMAFLineStripSourceTest
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFLineStripSourceTest_H__
#define __CPP_UNIT_vtkMAFLineStripSourceTest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkRenderWindow;
class vtkMAFLineStripSource;

//------------------------------------------------------------------------------
// Test class for vtkMAFLineStripSourceTest
//------------------------------------------------------------------------------
class vtkMAFLineStripSourceTest : public mafTest
{
	CPPUNIT_TEST_SUITE(vtkMAFLineStripSourceTest);
	CPPUNIT_TEST(TestDynamicAllocation);

	CPPUNIT_TEST_SUITE_END();

protected:
	void TestDynamicAllocation();
};

#endif
