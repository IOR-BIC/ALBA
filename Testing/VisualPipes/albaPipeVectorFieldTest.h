/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorFieldTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeVectorFieldTest_H__
#define __CPP_UNIT_albaPipeVectorFieldTest_H__

#include "albaTest.h"

class vtkRenderWindow;
class vtkRenderer;
class vtkRenderWindowInteractor;

class albaPipeVectorFieldTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeVectorFieldTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestCreate );
  CPPUNIT_TEST_SUITE_END();

protected:

  void TestFixture();
  void TestCreate();

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;
};

#endif
