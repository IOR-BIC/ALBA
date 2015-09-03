/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputMeshTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEOutputMeshTest.h"

#include "mafVMEOutputMesh.h"

#include "mafVMEMesh.h"
#include "mmaMaterial.h"

#include "mafMatrix.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkMath.h"
#include "vtkDelaunay3D.h"
#include "vtkMAFSmartPointer.h"

#include <iostream>

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void mafVMEOutputMeshTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputMeshTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputMesh outputSurface;
}
//----------------------------------------------------------------------------
void mafVMEOutputMeshTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputMesh *outputSurface = new mafVMEOutputMesh();
  cppDEL(outputSurface);
}
//----------------------------------------------------------------------------
void mafVMEOutputMeshTest::TestGetUnstructuredGridData()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> points;
  for (int i = 0; i < 25; i++)
  {
    points->InsertPoint(i, vtkMath::Random(0,1), vtkMath::Random(0,1), vtkMath::Random(0,1));	  
  }

  vtkMAFSmartPointer<vtkPolyData> pointsToTriangulate;
  pointsToTriangulate->SetPoints(points);

  //Delaunay3D is used to triangulate the points
  //the output of the filter is an unstructured grid
  vtkMAFSmartPointer<vtkDelaunay3D> triangulation;
  triangulation->SetInput(pointsToTriangulate);
  triangulation->SetTolerance(0.01);
  triangulation->SetAlpha(0.2);
  triangulation->BoundingTriangulationOff();
  triangulation->Update();

  //create a mesh
  mafVMEMesh *vmeMesh;
  mafNEW(vmeMesh);
  vmeMesh->SetData(triangulation->GetOutput(), -1);

  vmeMesh->GetOutput()->GetVTKData()->Update();
  vmeMesh->Update();

  mafVMEOutputMesh *outputMesh = NULL;
  outputMesh = (mafVMEOutputMesh*)vmeMesh->GetOutput();
  m_Result = (vtkDataSet*)outputMesh->GetUnstructuredGridData() == vmeMesh->GetOutput()->GetVTKData();

  TEST_RESULT;

  mafDEL(vmeMesh);
}

//----------------------------------------------------------------------------
void mafVMEOutputMeshTest::TestSetGetMaterial()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> points;
  for (int i = 0; i < 25; i++)
  {
    points->InsertPoint(i, vtkMath::Random(0,1), vtkMath::Random(0,1), vtkMath::Random(0,1));	  
  }

  vtkMAFSmartPointer<vtkPolyData> pointsToTriangulate;
  pointsToTriangulate->SetPoints(points);

  //Delaunay3D is used to triangulate the points
  //the output of the filter is an unstructured grid
  vtkMAFSmartPointer<vtkDelaunay3D> triangulation;
  triangulation->SetInput(pointsToTriangulate);
  triangulation->SetTolerance(0.01);
  triangulation->SetAlpha(0.2);
  triangulation->BoundingTriangulationOff();
  triangulation->Update();

  //create a mesh
  mafVMEMesh *vmeMesh;
  mafNEW(vmeMesh);
  vmeMesh->SetData(triangulation->GetOutput(), -1);

  vmeMesh->GetOutput()->GetVTKData()->Update();
  vmeMesh->Update();

  mafVMEOutputMesh *outputMesh = (mafVMEOutputMesh*)vmeMesh->GetOutput();
  mmaMaterial *material;

  m_Result = vmeMesh->GetMaterial() == outputMesh->GetMaterial();
  TEST_RESULT;
  material = outputMesh->GetMaterial();

  m_Result = outputMesh->GetMaterial() == material && vmeMesh->GetMaterial() == material;
  TEST_RESULT;
  
  mafDEL(vmeMesh);
}
