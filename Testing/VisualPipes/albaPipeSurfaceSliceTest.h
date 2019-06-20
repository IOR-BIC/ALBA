/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeSurfaceSliceTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaPipeSurfaceSliceTest_H__
#define __CPP_UNIT_albaPipeSurfaceSliceTest_H__

#include "albaTest.h"

class vtkPolyData;
class vtkRenderWindowInteractor;
class vtkProp;
class vtkPropCollection;

class albaPipeSurfaceSliceTest : public albaTest
{
public: 
  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaPipeSurfaceSliceTest );
  CPPUNIT_TEST(TestFixture); // just to test that the fixture has no leaks
  CPPUNIT_TEST( TestSurfacePipeCreation );
  CPPUNIT_TEST( TestSetGetThickness );
  CPPUNIT_TEST( TestSurfacePipeExecution );
  CPPUNIT_TEST( TestCloudPipeExecution );
 
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestFixture();
  /** Test the pipe with a albaVMESurface as input */
  void TestSurfacePipeExecution();
  /** Test the pipe with a albaVMELandamarkCloud as input */
  void TestCloudPipeExecution();
  /** Test pipe creation. */
  void TestSurfacePipeCreation();
  /** Test Set and Get methods for thickness attribute. */
  void TestSetGetThickness();

  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif
