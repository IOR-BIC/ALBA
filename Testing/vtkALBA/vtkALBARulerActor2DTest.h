/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARulerActor2DTest
 Authors: Alberto Losi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkALBARulerActor2DTest_H__
#define __CPP_UNIT_vtkALBARulerActor2DTest_H__

#include "albaTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------

class vtkImageData;
class vtkActor2D;

/**
  Class Name: vtkALBAGridActorTest.
  Test for vtkALBAGridActor.
*/
class vtkALBARulerActor2DTest : public albaTest
{
  public:
		// Executed before each test
		void BeforeTest();

		// Executed after each test
		void AfterTest();

    /** Test suite begin. */
    CPPUNIT_TEST_SUITE( vtkALBARulerActor2DTest );
    /** Instance creation and deletion test. */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test SetColor. */
    CPPUNIT_TEST( TestSetColor );
    /** Test SetLabelScaleVisibility. */
    CPPUNIT_TEST( TestSetLabelScaleVisibility );
    /** Test SetLabelAxesVisibility. */
    CPPUNIT_TEST( TestSetLabelAxesVisibility );
    /** Test SetAxesVisibility. */
    CPPUNIT_TEST( TestSetAxesVisibility );
    /** Test SetTickVisibility. */
    CPPUNIT_TEST( TestSetTickVisibility );
    /** Test SetLegend. */
    CPPUNIT_TEST( TestSetLegend );
    /**Test SetScaleFactor and GetScaleFactor. */
    CPPUNIT_TEST( TestSetGetScaleFactor );
    /** Test UseGlobalAxes. */
    CPPUNIT_TEST( TestUseGlobalAxes );
    /** Test SetInverseTicks. */
    CPPUNIT_TEST( TestSetInverseTicks );
    /**Test SetAttachPositionFlag. */
    /** Test SetAttachPosition. */
    CPPUNIT_TEST( TestSetAttachPosition );
    /** Test ChangeRulerMarginsAndLengths. */
    CPPUNIT_TEST( TestChangeRulerMarginsAndLengths );
    /** Test SetText. */
    CPPUNIT_TEST( TestSetText );
    /** Test PrintSelf. */
    CPPUNIT_TEST( TestPrintSelf );
    /** Test suite end. */
    CPPUNIT_TEST_SUITE_END();

  //protected:
public:
  /** Test the object creation and deletion.*/
  void TestDynamicAllocation();
  void TestSetColor();
  void TestSetLabelScaleVisibility();
  void TestSetLabelAxesVisibility();
  void TestSetAxesVisibility();
  void TestSetTickVisibility();
  void TestSetLegend();
  void TestSetGetScaleFactor();
  void TestUseGlobalAxes();
  void TestSetInverseTicks();
  void TestSetAttachPosition();
  void TestChangeRulerMarginsAndLengths();
  void TestSetText();
  /** Call PrintSelf in order to print information in console output.*/
  void TestPrintSelf();

	/** Render data in vtk render window.*/
	void RenderData(vtkActor2D *actor, char* testName);
};

#endif
