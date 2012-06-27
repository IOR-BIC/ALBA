/*=========================================================================

 Program: MAF2
 Module: mafVMEMeshTest
 Authors: Stefano Perticoni
 
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

#include "mafVMEMeshTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "mafVMEGeneric.h"
#include "mafVMEMesh.h"
#include "vtkMAFSmartPointer.h"

#include "vtkConeSource.h"
#include "vtkPoints.h"
#include "vtkDelaunay3D.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"



void mafVMEMeshTest::setUp()
{
  	
}

void mafVMEMeshTest::tearDown()
{
  
}

void mafVMEMeshTest::TestFixture()
{

}

void mafVMEMeshTest::TestSetData()
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

  // to be used later...
  int cellsNumber = triangulation->GetOutput()->GetNumberOfCells();

  mafVMEMesh *vmeMesh;

  // create vme volume
  mafNEW(vmeMesh);

  // try to set this data to the volume
  int returnValue = -1;

  // no leaks in this one...
  returnValue = vmeMesh->SetData(triangulation->GetOutput(), -1);
  CPPUNIT_ASSERT(returnValue == MAF_OK);

  vtkDataSet *data = vmeMesh->GetUnstructuredGridOutput()->GetVTKData();
  CPPUNIT_ASSERT(data);

  // update the data... 
  data->Update();
  // ... otherwise this will fail!
  CPPUNIT_ASSERT_EQUAL(cellsNumber, data->GetNumberOfCells());

  // destroy vme
  mafDEL(vmeMesh);
  
}
