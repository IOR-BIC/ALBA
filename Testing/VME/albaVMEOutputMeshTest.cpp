/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputMeshTest
 Authors: Roberto Mucci
 
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
#include "albaVMEOutputMeshTest.h"

#include "albaVMEOutputMesh.h"

#include "albaVMEMesh.h"
#include "mmaMaterial.h"

#include "albaMatrix.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include "vtkDelaunay3D.h"
#include "vtkALBASmartPointer.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEOutputMeshTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputMeshTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputMesh outputSurface;
}
//----------------------------------------------------------------------------
void albaVMEOutputMeshTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputMesh *outputSurface = new albaVMEOutputMesh();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void albaVMEOutputMeshTest::TestGetUnstructuredGridData()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> points;
  for (int i = 0; i < 25; i++)
  {
    points->InsertPoint(i, vtkMath::Random(0,1), vtkMath::Random(0,1), vtkMath::Random(0,1));	  
  }

  vtkALBASmartPointer<vtkPolyData> pointsToTriangulate;
  pointsToTriangulate->SetPoints(points);

  //Delaunay3D is used to triangulate the points
  //the output of the filter is an unstructured grid
  vtkALBASmartPointer<vtkDelaunay3D> triangulation;
  triangulation->SetInput(pointsToTriangulate);
  triangulation->SetTolerance(0.01);
  triangulation->SetAlpha(0.2);
  triangulation->BoundingTriangulationOff();
  triangulation->Update();

  //create a mesh
  albaVMEMesh *vmeMesh;
  albaNEW(vmeMesh);
  vmeMesh->SetData(triangulation->GetOutput(), -1);

  vmeMesh->GetOutput()->GetVTKData()->Update();
  vmeMesh->Update();

  albaVMEOutputMesh *outputMesh = NULL;
  outputMesh = (albaVMEOutputMesh*)vmeMesh->GetOutput();
  m_Result = (vtkDataSet*)outputMesh->GetUnstructuredGridData() == vmeMesh->GetOutput()->GetVTKData();

  TEST_RESULT;

  albaDEL(vmeMesh);
}

//----------------------------------------------------------------------------
void albaVMEOutputMeshTest::TestSetGetMaterial()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> points;
  for (int i = 0; i < 25; i++)
  {
    points->InsertPoint(i, vtkMath::Random(0,1), vtkMath::Random(0,1), vtkMath::Random(0,1));	  
  }

  vtkALBASmartPointer<vtkPolyData> pointsToTriangulate;
  pointsToTriangulate->SetPoints(points);

  //Delaunay3D is used to triangulate the points
  //the output of the filter is an unstructured grid
  vtkALBASmartPointer<vtkDelaunay3D> triangulation;
  triangulation->SetInput(pointsToTriangulate);
  triangulation->SetTolerance(0.01);
  triangulation->SetAlpha(0.2);
  triangulation->BoundingTriangulationOff();
  triangulation->Update();

  //create a mesh
  albaVMEMesh *vmeMesh;
  albaNEW(vmeMesh);
  vmeMesh->SetData(triangulation->GetOutput(), -1);

  vmeMesh->GetOutput()->GetVTKData()->Update();
  vmeMesh->Update();

  albaVMEOutputMesh *outputMesh = (albaVMEOutputMesh*)vmeMesh->GetOutput();
  mmaMaterial *material;

	material = vmeMesh->GetMaterial();

  m_Result = material == outputMesh->GetMaterial();
  TEST_RESULT;
  material = outputMesh->GetMaterial();

  m_Result = outputMesh->GetMaterial() == material && vmeMesh->GetMaterial() == material;
  TEST_RESULT;
  
  albaDEL(vmeMesh);
}
