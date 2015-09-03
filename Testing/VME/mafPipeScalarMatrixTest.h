/*=========================================================================

 Program: MAF2
 Module: mafPipeScalarMatrixTest
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeScalarMatrixTest_H__
#define __CPP_UNIT_mafPipeScalarMatrixTest_H__

#include "mafTest.h"

class vtkPolyData;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class mafPipeScalarMatrixTest : public mafTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafPipeScalarMatrixTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST_SUITE_END();

protected:
  /* Just to test that the fixture has no leaks **/
  void TestFixture();

  /** Test the pipe execution */
  void TestPipeExecution();

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;

  /**Compare a list of already saved images (control images) with renderized image from render window
  @scalarIndex: index X of the image. Original Image is imageX.jpg, created imaged is compX.jpg
  */
  void CompareImages();
};

#endif
