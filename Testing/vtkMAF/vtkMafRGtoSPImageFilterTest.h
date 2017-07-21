/*=========================================================================

Program: MAF2
Module:  vtkMafRGtoSPImageFilterTest.h
Authors: Nicola Vanella
Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMafRGtoSPImageFilterTest_h
#define __vtkMafRGtoSPImageFilterTest_h

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------

class vtkMafRGtoSPImageFilterTest : public mafTest
{
public:
	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

	/** Test suite begin. */
	CPPUNIT_TEST_SUITE(vtkMafRGtoSPImageFilterTest);
	/** Instance creation and deletion test. */
	CPPUNIT_TEST(TestDynamicAllocation);
	/** Test 1. */
	CPPUNIT_TEST(FilterTest);
	/** Test PrintSelf. */
	//CPPUNIT_TEST(TestPrintSelf);
	/** Test suite end. */
	CPPUNIT_TEST_SUITE_END();

	//protected:
public:
	/** Test the object creation and deletion.*/
	void TestDynamicAllocation();

	void FilterTest();

	/** Call PrintSelf in order to print information in console output.*/
	void TestPrintSelf();
};

#endif
