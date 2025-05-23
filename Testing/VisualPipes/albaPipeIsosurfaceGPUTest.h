/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurfaceGPUTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeIsosurfaceGPUTest_H__
#define __CPP_UNIT_albaPipeIsosurfaceGPUTest_H__

#include "albaTest.h"

class vtkRenderWindowInteractor;
class vtkRectilinearGrid;

class albaPipeIsosurfaceGPUTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeIsosurfaceGPUTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestPipeExecutionCountour );
  CPPUNIT_TEST( TestPipeExecutionOpacity );
  CPPUNIT_TEST( TestExtractIsosurface );
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  void TestPipeExecutionCountour();
  void TestPipeExecutionOpacity();
  void TestExtractIsosurface();

  vtkRenderWindowInteractor *m_RenderWindowInteractor;
};

#endif
