/*=========================================================================

 Program: MAF2
 Module: vtkMAFProfilingActorTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFProfilingActorTest_H__
#define __CPP_UNIT_vtkMAFProfilingActorTest_H__

#include "mafTest.h"

//------------------------------------------------------------------------------
// Forward Declaration:
//------------------------------------------------------------------------------

class vtkImageData;
class vtkMAFProfilingActor;

/**
  Class Name: vtkMAFProfilingActorTest.
  Test for vtkMAFProfilingActor.
*/
class vtkMAFProfilingActorTest : public mafTest
{
  public:
		// Executed before each test
		void BeforeTest();

		// Executed after each test
		void AfterTest();

    /** Test suite begin. */
    CPPUNIT_TEST_SUITE( vtkMAFProfilingActorTest );
    /** Instance creation and deletion test. */
    CPPUNIT_TEST( TestDynamicAllocation );
    /** Test RenderOverlay. */
    CPPUNIT_TEST( TestRenderOverlay );
    /** Test RenderOpaqueGeometry. */
    CPPUNIT_TEST( TestRenderOpaqueGeometry );
    /** Test RenderTranslucentGeometry. */
    CPPUNIT_TEST( TestRenderTranslucentGeometry );
    /** Test PrintSelf. */
    CPPUNIT_TEST( TestPrintSelf );
    CPPUNIT_TEST_SUITE_END();

  protected:

    /** Test the object creation and deletion.*/
    void TestDynamicAllocation();

    /** Test RenderOverlay.*/
    void TestRenderOverlay();
    /** Test RenderOpaqueGeometry.*/
    void TestRenderOpaqueGeometry();
    /** Test RenderTranslucentGeometry.*/
    void TestRenderTranslucentGeometry();
    /** Test PrintSelf.*/
    void TestPrintSelf();
};

#endif
