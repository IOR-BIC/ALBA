/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEOutputSurfaceTest.cpp,v $
Language:  C++
Date:      $Date: 2009-02-27 15:32:36 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEOutputSurfaceTest.h"

#include "mafVMEOutputSurface.h"

#include "mafVMESurfaceParametric.h"
#include "mmaMaterial.h"

#include "mafMatrix.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::setUp()
//----------------------------------------------------------------------------
{
	m_Result = false;
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputSurface outputSurface;
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputSurface *outputSurface = new mafVMEOutputSurface();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::TestGetSurfaceData()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurface;
  mafNEW(vmeParametricSurface);
  vmeParametricSurface->GetOutput()->GetVTKData()->Update();
  vmeParametricSurface->Update();

  mafVMEOutputSurface *outputSurface = NULL;
  outputSurface = vmeParametricSurface->GetSurfaceOutput();
  m_Result = outputSurface->GetSurfaceData() == vmeParametricSurface->GetOutput()->GetVTKData();

  TEST_RESULT;

  mafDEL(vmeParametricSurface);
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::TestSetGetTexture()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurface;
  mafNEW(vmeParametricSurface);
  vmeParametricSurface->GetOutput()->GetVTKData()->Update();
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
  imageData->Update();

  mafVMEOutputSurface *surfaceOutput;
  surfaceOutput = vmeParametricSurface->GetSurfaceOutput();

  m_Result = NULL == surfaceOutput->GetTexture();
  TEST_RESULT;

  surfaceOutput->SetTexture(imageData);
  surfaceOutput->Update();

  m_Result = imageData == surfaceOutput->GetTexture();

  TEST_RESULT;

  mafDEL(vmeParametricSurface);
  vtkDEL(imageData);
  vtkDEL(array);
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::TestSetGetMaterial()
//----------------------------------------------------------------------------
{
  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurface;
  mafNEW(vmeParametricSurface);
  vmeParametricSurface->GetOutput()->GetVTKData()->Update();
  vmeParametricSurface->Update();

  mafVMEOutputSurface *outputSurface = vmeParametricSurface->GetSurfaceOutput();
  mmaMaterial *material;

  m_Result = vmeParametricSurface->GetMaterial() == outputSurface->GetMaterial();
  TEST_RESULT;
  material = outputSurface->GetMaterial();


  m_Result = outputSurface->GetMaterial() == material && vmeParametricSurface->GetMaterial() == material;
  TEST_RESULT;
  

  mafDEL(vmeParametricSurface);
}
//----------------------------------------------------------------------------
void mafVMEOutputSurfaceTest::TestGetNumberOfTrainglesUpdate()
//----------------------------------------------------------------------------
{
	//create a parametric surface
	mafVMESurfaceParametric *vmeParametricSurface;
	mafNEW(vmeParametricSurface);
	vmeParametricSurface->GetOutput()->GetVTKData()->Update();
	vmeParametricSurface->Update();
  int controlNumberOfPolys = vtkPolyData::SafeDownCast(vmeParametricSurface->GetOutput()->GetVTKData())->GetNumberOfPolys();

  mafVMEOutputSurface *surfaceOutput;
  surfaceOutput = vmeParametricSurface->GetSurfaceOutput();
  mafString correctValue;
  correctValue << controlNumberOfPolys;
  m_Result = !mafString(surfaceOutput->GetNumberOfTriangles()).Equals(correctValue); //update not already applied
  TEST_RESULT;

  surfaceOutput->Update();

  m_Result = mafString(surfaceOutput->GetNumberOfTriangles()).Equals(correctValue); //update not already applied
  TEST_RESULT;
	
	mafDEL(vmeParametricSurface);
}