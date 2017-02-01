/*=========================================================================

 Program: MAF2
 Module: mafVisualPipeSlicerSliceTest
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafVisualPipeSlicerSliceTest_H__
#define __CPP_UNIT_mafVisualPipeSlicerSliceTest_H__

#include "mafTest.h"

#include "vtkDataSet.h"
#include "vtkTransform.h"
#include "vtkRenderWindowInteractor.h"

class mafVisualPipeSlicerSliceTest : public mafTest
{
public:

  // Executed before each test
  void BeforeTest();

  // Executed after each test
  void AfterTest();

  CPPUNIT_TEST_SUITE( mafVisualPipeSlicerSliceTest );
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
