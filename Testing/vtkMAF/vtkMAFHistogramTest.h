/*=========================================================================

 Program: MAF2
 Module: vtkMAFHistogramTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFHistogramtest_H__
#define __CPP_UNIT_vtkMAFHistogramtest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------

class vtkImageData;

/**
  Class Name: vtkMAFHistogramTest.
  Test for vtkMAFHistogram.
*/
class vtkMAFHistogramTest : public mafTest
{
  public:
		// Executed before each test
		void BeforeTest();

		// Executed after each test
		void AfterTest();

    /** Test suite begin. */
    CPPUNIT_TEST_SUITE( vtkMAFHistogramTest );
    /** Instance creation and deletion test. */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test Point Representation */
    CPPUNIT_TEST( TestHistogramPointRepresentation );
    /** Test Line Representation */
    CPPUNIT_TEST( TestHistogramLineRepresentation );
    /** Test Bar Representation */
    CPPUNIT_TEST( TestHistogramBarRepresentation );
    /** Test Histogram Logaritmic Scale Properties*/
    CPPUNIT_TEST( TestHistogramLogaritmicProperties );
    /** Test Histogram Scale Factor and Color Properties*/
    CPPUNIT_TEST( TestHistogramScaleFactorColorProperties );
    /** Test Histogram Get Attributes*/
    CPPUNIT_TEST( TestHistogramGetAttributesAndUpdateLines );
    /** Test PrintSelf. */
    CPPUNIT_TEST( TestPrintSelf );
    /** Test suite end. */
    CPPUNIT_TEST_SUITE_END();

  protected:

    /** Test the object creation and deletion.*/
    void TestDynamicAllocation();
    /** Test Histogram with Point Representation*/
    void TestHistogramPointRepresentation();
    /** Test Histogram with Line Representation*/
    void TestHistogramLineRepresentation();
    /** Test Histogram with Bar Representation*/
    void TestHistogramBarRepresentation();
    /** Test Histogram Logaritmic Scale Properties*/
    void TestHistogramLogaritmicProperties();
    /** Test Histogram Scale Factor and Color Properties*/
    void TestHistogramScaleFactorColorProperties();
    /** Test Histogram Get Attributes and Update Lines*/
    void TestHistogramGetAttributesAndUpdateLines();
    /** Call PrintSelf in order to print information in console output.*/
    void TestPrintSelf();

    /** Render data in vtk render window.*/
		void RenderData(vtkActor2D *actor, char* testName);
};

#endif
