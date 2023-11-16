/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeMeshTest
 Authors: Daniele Giunchi, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeMeshTest_H__
#define __CPP_UNIT_albaPipeMeshTest_H__

#include "albaTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class albaPipeMeshTest : public albaTest
{
  public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

	CPPUNIT_TEST_SUITE(albaPipeMeshTest);
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
	CPPUNIT_TEST(TestPipeExecution);
	CPPUNIT_TEST(TestScalarVisualization);
	CPPUNIT_TEST(TestPipeDensityMap);
	CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
	void TestPipeExecution();
	void TestScalarVisualization();
	void TestPipeDensityMap();

  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /**Compare Mapper scalarange with controlled values */
  void ProceduralControl(double controlRangeMapper[2] , vtkProp *propToControl);

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif
