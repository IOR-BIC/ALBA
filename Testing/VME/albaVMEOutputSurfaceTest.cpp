/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputSurfaceTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaVMEOutputSurfaceTest.h"

#include "albaVMEOutputSurface.h"

#include "albaVMESurfaceParametric.h"
#include "mmaMaterial.h"

#include "albaMatrix.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEOutputSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputSurface outputSurface;
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputSurface *outputSurface = new albaVMEOutputSurface();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceTest::TestGetSurfaceData()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurface;
  albaNEW(vmeParametricSurface);
  vmeParametricSurface->Update();

  albaVMEOutputSurface *outputSurface = NULL;
  outputSurface = vmeParametricSurface->GetSurfaceOutput();
  m_Result = outputSurface->GetSurfaceData() == vmeParametricSurface->GetOutput()->GetVTKData();

  TEST_RESULT;

  albaDEL(vmeParametricSurface);
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceTest::TestSetGetTexture()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurface;
  albaNEW(vmeParametricSurface);
  vmeParametricSurface->Update();

  vtkIntArray *array;
  vtkNEW(array);
  int i = 0;
  int dimensions[3] = {3,3,1};
  for(; i < dimensions[0] * dimensions[1] * dimensions[2]; i++)
  {
    array->InsertNextTuple1(i);
  }

  vtkImageData *imageData;
  vtkNEW(imageData);
  imageData->SetDimensions(dimensions);
  
  imageData->GetPointData()->SetScalars(array);

  albaVMEOutputSurface *surfaceOutput;
  surfaceOutput = vmeParametricSurface->GetSurfaceOutput();

  m_Result = NULL == surfaceOutput->GetTexture();
  TEST_RESULT;

  surfaceOutput->SetTexture(imageData);
  surfaceOutput->Update();

  m_Result = imageData == surfaceOutput->GetTexture();

  TEST_RESULT;

  albaDEL(vmeParametricSurface);
  vtkDEL(imageData);
  vtkDEL(array);
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceTest::TestSetGetMaterial()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurface;
  albaNEW(vmeParametricSurface);
  vmeParametricSurface->Update();

  albaVMEOutputSurface *outputSurface = vmeParametricSurface->GetSurfaceOutput();
  mmaMaterial *material;

  m_Result = vmeParametricSurface->GetMaterial() == outputSurface->GetMaterial();
  TEST_RESULT;
  material = outputSurface->GetMaterial();


  m_Result = outputSurface->GetMaterial() == material && vmeParametricSurface->GetMaterial() == material;
  TEST_RESULT;
  

  albaDEL(vmeParametricSurface);
}
//----------------------------------------------------------------------------
void albaVMEOutputSurfaceTest::TestGetNumberOfTrainglesUpdate()
//----------------------------------------------------------------------------
{
	//create a parametric surface
	albaVMESurfaceParametric *vmeParametricSurface;
	albaNEW(vmeParametricSurface);
	vmeParametricSurface->Update();
  int controlNumberOfPolys = vtkPolyData::SafeDownCast(vmeParametricSurface->GetOutput()->GetVTKData())->GetNumberOfPolys();

  albaVMEOutputSurface *surfaceOutput;
  surfaceOutput = vmeParametricSurface->GetSurfaceOutput();
  albaString correctValue;
  correctValue << controlNumberOfPolys;
  m_Result = !albaString(surfaceOutput->GetNumberOfTriangles()).Equals(correctValue); //update not already applied
  TEST_RESULT;

  surfaceOutput->Update();

  m_Result = albaString(surfaceOutput->GetNumberOfTriangles()).Equals(correctValue); //update not already applied
  TEST_RESULT;
	
	albaDEL(vmeParametricSurface);
}