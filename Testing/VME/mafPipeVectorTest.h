/*=========================================================================

 Program: MAF2
 Module: mafPipeVectorTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeVectorTest_H__
#define __CPP_UNIT_mafPipeVectorTest_H__

#include "mafTest.h"

class vtkPolyData;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class mafPipeVectorTest : public mafTest
{
  public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( mafPipeVectorTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestPipeExecution );
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
	void TestPipeExecution();

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /**Compare a list of already saved images (control images) with renderized image from render window
     @testIndex: index X of the image. Original Image is imageX.jpg, created imaged is compX.jpg  */
  void CompareImages(int testIndex = 0);

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif
