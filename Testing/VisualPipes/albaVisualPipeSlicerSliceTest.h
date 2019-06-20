/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipeSlicerSliceTest
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaVisualPipeSlicerSliceTest_H__
#define __CPP_UNIT_albaVisualPipeSlicerSliceTest_H__

#include "albaTest.h"

#include "vtkDataSet.h"
#include "vtkTransform.h"
#include "vtkRenderWindowInteractor.h"

class albaVisualPipeSlicerSliceTest : public albaTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( albaVisualPipeSlicerSliceTest );
  CPPUNIT_TEST( TestFixture);
  CPPUNIT_TEST( TestPipeExecution);
  CPPUNIT_TEST_SUITE_END();

protected:  
  void TestFixture();
  void TestPipeExecution();

  /** Comment by the programmer:
  The testPipeExecution method generates some leaks which I was not able to hide with VS9 under WS Vista.
  Please verify if this is the case with your compiler and in case try to solve the problem.
  */

  vtkRenderWindowInteractor *m_RenderWindowInteractor;

  /** Select The Actor that will be controlled*/
  vtkProp *SelectActorToControl(vtkPropCollection* propList, int index);
};

#endif
