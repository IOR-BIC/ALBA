/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeshSliceTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeMeshSliceTest_H__
#define __CPP_UNIT_albaPipeMeshSliceTest_H__

#include "albaTest.h"
#include "albaTestDefines.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class albaPipeMeshSliceTest : public albaTest
{
  public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE( albaPipeMeshSliceTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestPipeExecution );
  CPPUNIT_TEST( TestPipeExecution_Wireframe );
  CPPUNIT_TEST( TestPipeExecution_WiredActorVisibility );
  CPPUNIT_TEST( TestPipeExecution_FlipNormal );
  CPPUNIT_TEST( TestPipeExecution_UseVTKProperty );

  //TODO REENABLE THIS TEST 
#ifndef DISABLE_GPU_VOLUME_RENDERING_TESTS
  CPPUNIT_TEST( TestPipeExecution_Thickness_PickActor );
#endif
  	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
	void TestPipeExecution();
  void TestPipeExecution_Wireframe();
  void TestPipeExecution_WiredActorVisibility();
  void TestPipeExecution_FlipNormal();
  void TestPipeExecution_UseVTKProperty();
  void TestPipeExecution_Thickness_PickActor();

  /**Compare Mapper scalarange with controlled values */
  void ProceduralControl(double controlRangeMapper[2] , vtkProp *propToControl);

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif
