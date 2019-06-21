/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAGridActorTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBAGridActortest_H__
#define __CPP_UNIT_vtkALBAGridActortest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------
class vtkImageData;
class vtkActor;

/**
  Class Name: vtkALBAGridActorTest.
  Test for vtkALBAGridActor.
*/
class vtkALBAGridActorTest : public albaTest
{
public:
	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

	/** Test suite begin. */
	CPPUNIT_TEST_SUITE(vtkALBAGridActorTest);
	/** Instance creation and deletion test. */
	CPPUNIT_TEST(TestDynamicAllocation);
	/** Test SetGridNormal. */
	CPPUNIT_TEST(TestSetGridNormal);
	/** Test SetGridPosition. */
	CPPUNIT_TEST(TestSetGridPosition);
	/** Test SetGridColor. */
	CPPUNIT_TEST(TestSetGridColor);
	/** Test SetGetLabelActor. */
	CPPUNIT_TEST(TestGetLabelActor);
	/** Test PrintSelf. */
	CPPUNIT_TEST(TestPrintSelf);
	/** Test suite end. */
	CPPUNIT_TEST_SUITE_END();

protected:

	/** Test the object creation and deletion.*/
	void TestDynamicAllocation();
	/** Test SetGridNormal in order to see grid new perpendicular axis.*/
	void TestSetGridNormal();
	/** Test SetGridPosition in order to see grid position along its axis.*/
	void TestSetGridPosition();
	/** Test SetGridColor in order to see grid color.*/
	void TestSetGridColor();
	/** Test GetLabelActor in order to see which value of scale  as string has text actor.*/
	void TestGetLabelActor();
	/** Call PrintSelf in order to print information in console output.*/
	void TestPrintSelf();

	/** Render data in vtk render window.*/
	void RenderData(vtkActor *actor, char* testName);
};

#endif
