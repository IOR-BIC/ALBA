/*=========================================================================

 Program: MAF2
 Module: mafPipeLandmarkCloudTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafPipeLandmarkCloudTest_H__
#define __CPP_UNIT_mafPipeLandmarkCloudTest_H__

#include "mafTest.h"

class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;

/** Test for mafVMELandmarkCloud */
class mafPipeLandmarkCloudTest : public mafTest
{

public:
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafPipeLandmarkCloudTest );
  CPPUNIT_TEST( DynamicAllocationTest );
  CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST_SUITE_END();

protected:
  void DynamicAllocationTest();
  void TestPipeExecution();

  vtkRenderer *m_Renderer;
  vtkRenderWindow *m_RenderWindow;
  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /**Compare a list of already saved images (control images) with renderized image from render window
  @imageIndex: index X of the image. Original Image is imageX.jpg, created imaged is compX.jpg
  */
  void CompareImages(int imageIndex);

};

#endif
