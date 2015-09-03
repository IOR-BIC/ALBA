/*=========================================================================

 Program: MAF2
 Module: mafPipeTensorFieldGlyphsTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeTensorFieldGlyphsTest_H__
#define __CPP_UNIT_mafPipeTensorFieldGlyphsTest_H__

#include "mafTest.h"

class vtkRenderWindow;
class vtkRenderer;
class vtkRenderWindowInteractor;

class mafPipeTensorFieldGlyphsTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafPipeTensorFieldGlyphsTest );
  CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestCreate );
  CPPUNIT_TEST_SUITE_END();

protected:
  /**Compare an already saved image (control image) with renderized image from render window
  */
  void CompareImage();

  void TestFixture();
  void TestCreate();

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;
};

#endif
