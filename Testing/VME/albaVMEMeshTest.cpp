/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeshTest
 Authors: Stefano Perticoni
 
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

#include "albaVMEMeshTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "albaVMEGeneric.h"
#include "albaVMEMesh.h"
#include "vtkALBASmartPointer.h"

#include "vtkConeSource.h"
#include "vtkPoints.h"
#include "vtkDelaunay3D.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"


void albaVMEMeshTest::TestFixture()
{

}

void albaVMEMeshTest::TestSetData()
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
  triangulation->SetInputData(pointsToTriangulate);
  triangulation->SetTolerance(0.01);
  triangulation->SetAlpha(0.2);
  triangulation->BoundingTriangulationOff();
  triangulation->Update();

  // to be used later...
  vtkIdType cellsNumber = triangulation->GetOutput()->GetNumberOfCells();

  albaVMEMesh *vmeMesh;

  // create vme volume
  albaNEW(vmeMesh);

  // try to set this data to the volume
  int returnValue = -1;

  // no leaks in this one...
  returnValue = vmeMesh->SetData(triangulation->GetOutput(), -1);
  CPPUNIT_ASSERT(returnValue == ALBA_OK);

  vtkDataSet *data = vmeMesh->GetUnstructuredGridOutput()->GetVTKData();
  CPPUNIT_ASSERT(data);

  // ... otherwise this will fail!
  CPPUNIT_ASSERT_EQUAL(cellsNumber, data->GetNumberOfCells());

  // destroy vme
  albaDEL(vmeMesh);
  
}
