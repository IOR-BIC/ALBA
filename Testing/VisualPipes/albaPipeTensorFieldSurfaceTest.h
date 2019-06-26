/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeTensorFieldSurfaceTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeTensorFieldSurfaceTest_H__
#define __CPP_UNIT_albaPipeTensorFieldSurfaceTest_H__

#include "albaTest.h"

class vtkRenderWindowInteractor;

class albaPipeTensorFieldSurfaceTest : public albaTest
{
public: 
	// Executed before each test
	void BeforeTest();

	// Executed after each test
	void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeTensorFieldSurfaceTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestCreate );
  CPPUNIT_TEST_SUITE_END();

protected:

	vtkRenderWindowInteractor *m_RenderWindowInteractor;

  void TestFixture();
  void TestCreate();
};
#endif
