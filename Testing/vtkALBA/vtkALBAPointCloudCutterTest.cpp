/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPointCloudCutterTest
 Authors: Test Suite

 Copyright (c) BIC
 All rights reserved. See Copyright.txt or

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAPointCloudCutterTest.h"
#include "vtkALBAPointCloudCutter.h"
#include "vtkALBASmartPointer.h"
#include <iostream>

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestFixture()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;
  CPPUNIT_ASSERT(true);
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestSetCutFunction()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;
  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  // Set the cut function
  cutter->SetCutFunction(plane);

  // Verify plane properties are accessible
  vtkPlane *retrievedPlane = vtkPlane::SafeDownCast(cutter->GetCutFunction());
  CPPUNIT_ASSERT(retrievedPlane != NULL);
  
  // Verify origin
  double origin[3];
  retrievedPlane->GetOrigin(origin);
  CPPUNIT_ASSERT(origin[0] == 0.0);
  CPPUNIT_ASSERT(origin[1] == 0.0);
  CPPUNIT_ASSERT(origin[2] == 0.0);
  
  // Verify normal
  double normal[3];
  retrievedPlane->GetNormal(normal);
  CPPUNIT_ASSERT(normal[0] == 0.0);
  CPPUNIT_ASSERT(normal[1] == 0.0);
  CPPUNIT_ASSERT(normal[2] == 1.0);
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestGetCutFunction()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;
  vtkALBASmartPointer<vtkPlane> plane;
  
  // Initially should be NULL
  CPPUNIT_ASSERT(cutter->GetCutFunction() == NULL);

  // Set and verify
  cutter->SetCutFunction(plane);
  CPPUNIT_ASSERT(cutter->GetCutFunction() != NULL);
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestInitialize()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  // Create simple input
  vtkPolyData *input = CreateSimplePointCloud(10);
  cutter->SetInputData(input);

  // Execute to populate output
  cutter->Update();

  // Get output
  vtkPolyData *output = cutter->GetOutput();
  CPPUNIT_ASSERT(output != NULL);

  // Now initialize should clear the output
  cutter->Initialize();
  CPPUNIT_ASSERT(output->GetNumberOfPoints() == 0);

  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestSlicePointsSimple()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  // Create point cloud at z=0 (in plane)
  vtkPolyData *input = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *verts = vtkCellArray::New();

  for (int i = -2; i <= 2; i++)
  {
    for (int j = -2; j <= 2; j++)
    {
      vtkIdType id = points->InsertNextPoint(i, j, 0.0);
      verts->InsertNextCell(1, &id);
    }
  }

  input->SetPoints(points);
  input->SetVerts(verts);

  // Create plane at z=0 with normal pointing in z direction
  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  cutter->SetInputData(input);
  cutter->SetCutFunction(plane);
  cutter->Update();

  vtkPolyData *output = cutter->GetOutput();
  CPPUNIT_ASSERT(output != NULL);
  CPPUNIT_ASSERT(output->GetNumberOfPoints() == 25); // 5x5 grid

  points->Delete();
  verts->Delete();
  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestSlicePointsWithTolerance()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  // Create point cloud around z=0
  vtkPolyData *input = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *verts = vtkCellArray::New();

  // Points at z = 0.02 (within default tolerance of 0.05)
  for (int i = 0; i < 5; i++)
  {
    vtkIdType id = points->InsertNextPoint(i, 0.0, 0.02);
    verts->InsertNextCell(1, &id);
  }

  // Points at z = 0.1 (beyond default tolerance)
  for (int i = 0; i < 5; i++)
  {
    vtkIdType id = points->InsertNextPoint(i, 1.0, 0.1);
    verts->InsertNextCell(1, &id);
  }

  input->SetPoints(points);
  input->SetVerts(verts);

  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  cutter->SetInputData(input);
  cutter->SetCutFunction(plane);
  cutter->Update();

  vtkPolyData *output = cutter->GetOutput();
  CPPUNIT_ASSERT(output != NULL);
  CPPUNIT_ASSERT(output->GetNumberOfPoints() == 5); // Only first 5 points within tolerance

  points->Delete();
  verts->Delete();
  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestSlicePointsWithTransform()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  // Create point cloud at z=0
  vtkPolyData *input = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *verts = vtkCellArray::New();

  for (int i = -1; i <= 1; i++)
  {
    vtkIdType id = points->InsertNextPoint(i, 0.0, 0.0);
    verts->InsertNextCell(1, &id);
  }

  input->SetPoints(points);
  input->SetVerts(verts);

  // Create plane with transform
  vtkALBASmartPointer<vtkTransform> transform;
  transform->Translate(1.0, 1.0, 1.0);
  transform->RotateZ(45.0);

  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(1.0, 1.0, 1.0);
  plane->SetNormal(0.0, 0.0, 1.0);
  plane->SetTransform(transform);

  cutter->SetInputData(input);
  cutter->SetCutFunction(plane);
  cutter->Update();

  vtkPolyData *output = cutter->GetOutput();
  CPPUNIT_ASSERT(output != NULL);

  points->Delete();
  verts->Delete();
  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestGetIntersectionOfBoundsWithPlane()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  // Create point cloud with known bounds
  vtkPolyData *input = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *verts = vtkCellArray::New();

  // Create a cube from -1 to 1 in all dimensions
  for (int i = -1; i <= 1; i++)
  {
    for (int j = -1; j <= 1; j++)
    {
      for (int k = -1; k <= 1; k++)
      {
        vtkIdType id = points->InsertNextPoint(i, j, k);
        verts->InsertNextCell(1, &id);
      }
    }
  }

  input->SetPoints(points);
  input->SetVerts(verts);

  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  cutter->SetInputData(input);
  cutter->SetCutFunction(plane);
	cutter->Update();

  // Plane should intersect the bounding box
  double origin[3] = {0.0, 0.0, 0.0};
  double normal[3] = {0.0, 0.0, 1.0};

  bool intersects = cutter->GetIntersectionOfBoundsWithPlane(origin, normal);
  CPPUNIT_ASSERT(intersects == true);

  points->Delete();
  verts->Delete();
  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestGetIntersectionOfBoundsWithPlane_NoIntersection()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  // Create point cloud
  vtkPolyData *input = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *verts = vtkCellArray::New();

  // Cube from 0 to 1
  for (int i = 0; i <= 1; i++)
  {
    for (int j = 0; j <= 1; j++)
    {
      for (int k = 0; k <= 1; k++)
      {
        vtkIdType id = points->InsertNextPoint(i, j, k);
        verts->InsertNextCell(1, &id);
      }
    }
  }

  input->SetPoints(points);
  input->SetVerts(verts);

  cutter->SetInputData(input);
  cutter->Update();

  // Plane far away from the bounding box (z=100)
  double origin[3] = {0.0, 0.0, 100.0};
  double normal[3] = {0.0, 0.0, 1.0};

  bool intersects = cutter->GetIntersectionOfBoundsWithPlane(origin, normal);
  CPPUNIT_ASSERT(intersects == false);

  points->Delete();
  verts->Delete();
  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestTransferScalars()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  vtkPolyData *input = CreatePointCloudWithScalars(9, 1.0);

  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  cutter->SetInputData(input);
  cutter->SetCutFunction(plane);
  cutter->Update();

  vtkPolyData *output = cutter->GetOutput();
  CPPUNIT_ASSERT(output != NULL);
  CPPUNIT_ASSERT(output->GetPointData()->GetScalars() != NULL);
  CPPUNIT_ASSERT(output->GetNumberOfPoints() == 9);

  // Verify scalar values were transferred
  if (output->GetNumberOfPoints() > 0)
  {
    double scalarValue = output->GetPointData()->GetScalars()->GetComponent(0, 0);
    CPPUNIT_ASSERT(scalarValue >= 0.0);
  }

  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestTransferScalarsMultipleArrays()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  vtkPolyData *input = CreatePointCloudWithMultipleArrays(9, 1.0);

  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  cutter->SetInputData(input);
  cutter->SetCutFunction(plane);
  cutter->Update();

  vtkPolyData *output = cutter->GetOutput();
  CPPUNIT_ASSERT(output != NULL);
  CPPUNIT_ASSERT(output->GetPointData()->GetNumberOfArrays() >= 2);

  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestCreateSlice()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  vtkPolyData *input = CreateSimplePointCloud(9, 1.0);

  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  cutter->SetInputData(input);
  cutter->SetCutFunction(plane);
  cutter->Update();

  vtkPolyData *output = cutter->GetOutput();
  CPPUNIT_ASSERT(output != NULL);
  CPPUNIT_ASSERT(output->GetNumberOfPoints() > 0);

  input->Delete();
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestGetMTime()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  unsigned long mTime1 = cutter->GetMTime();

  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(0.0, 0.0, 0.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  cutter->SetCutFunction(plane);

  unsigned long mTime2 = cutter->GetMTime();

  // MTime should not decrease
  CPPUNIT_ASSERT(mTime2 >= mTime1);

  // Modify plane - cutter's mTime should update
  plane->SetOrigin(1.0, 1.0, 1.0);
  unsigned long mTime3 = cutter->GetMTime();

  CPPUNIT_ASSERT(mTime3 >= mTime2);
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutterTest::TestPrintSelf()
{
  vtkALBASmartPointer<vtkALBAPointCloudCutter> cutter;

  vtkALBASmartPointer<vtkPlane> plane;
  plane->SetOrigin(1.0, 2.0, 3.0);
  plane->SetNormal(0.0, 0.0, 1.0);

  vtkPolyData *input = CreateSimplePointCloud(5);
  cutter->SetInputData(input);
  cutter->SetCutFunction(plane);
  cutter->Update();

  // PrintSelf should not crash
  std::stringstream ss;
  cutter->PrintSelf(ss, vtkIndent());

  CPPUNIT_ASSERT(ss.str().length() > 0);

  input->Delete();
}

//------------------------------------------------------------------------------
// Helper functions
//------------------------------------------------------------------------------

vtkPolyData* vtkALBAPointCloudCutterTest::CreateSimplePointCloud(int numPoints, double spacing)
{
  vtkPolyData *polydata = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *verts = vtkCellArray::New();

  int side = (int)sqrt(numPoints);
  for (int i = 0; i < side; i++)
  {
    for (int j = 0; j < side; j++)
    {
      vtkIdType id = points->InsertNextPoint(i * spacing - (side - 1) * spacing / 2.0,
                                              j * spacing - (side - 1) * spacing / 2.0,
                                              0.0);
      verts->InsertNextCell(1, &id);
    }
  }

  polydata->SetPoints(points);
  polydata->SetVerts(verts);

  points->Delete();
  verts->Delete();

  return polydata;
}

vtkPolyData* vtkALBAPointCloudCutterTest::CreatePointCloudWithScalars(int numPoints, double spacing)
{
  vtkPolyData *polydata = CreateSimplePointCloud(numPoints, spacing);

  vtkFloatArray *scalars = vtkFloatArray::New();
  scalars->SetName("Scalars");
  scalars->SetNumberOfTuples(polydata->GetNumberOfPoints());

  for (int i = 0; i < polydata->GetNumberOfPoints(); i++)
  {
    scalars->SetValue(i, i * 1.5f);
  }

  polydata->GetPointData()->SetScalars(scalars);
  scalars->Delete();

  return polydata;
}

vtkPolyData* vtkALBAPointCloudCutterTest::CreatePointCloudWithMultipleArrays(int numPoints, double spacing)
{
  vtkPolyData *polydata = CreatePointCloudWithScalars(numPoints, spacing);

  vtkFloatArray *array2 = vtkFloatArray::New();
  array2->SetName("Array2");
  array2->SetNumberOfComponents(3);
  array2->SetNumberOfTuples(polydata->GetNumberOfPoints());

  for (int i = 0; i < polydata->GetNumberOfPoints(); i++)
  {
    array2->SetTuple3(i, i * 0.1f, i * 0.2f, i * 0.3f);
  }

  polydata->GetPointData()->AddArray(array2);
  array2->Delete();

  return polydata;
}

vtkPlane* vtkALBAPointCloudCutterTest::CreatePlane(double originX, double originY, double originZ,
                                                    double normalX, double normalY, double normalZ)
{
  vtkPlane *plane = vtkPlane::New();
  plane->SetOrigin(originX, originY, originZ);
  plane->SetNormal(normalX, normalY, normalZ);
  return plane;
}

bool vtkALBAPointCloudCutterTest::PointsEqual(double *v0, double *v1, double tolerance)
{
  double diff_x = v0[0] - v1[0];
  double diff_y = v0[1] - v1[1];
  double diff_z = v0[2] - v1[2];
  double distance = sqrt(diff_x * diff_x + diff_y * diff_y + diff_z * diff_z);
  return distance < tolerance;
}

bool vtkALBAPointCloudCutterTest::PointInPlane(double *point, double *origin, double *normal, double tolerance)
{
  double distance = DistancePointToPlane(point, origin, normal);
  return fabs(distance) < tolerance;
}

double vtkALBAPointCloudCutterTest::DistancePointToPlane(double *point, double *origin, double *normal)
{
  double toPoint[3];
  toPoint[0] = point[0] - origin[0];
  toPoint[1] = point[1] - origin[1];
  toPoint[2] = point[2] - origin[2];

  double distance = toPoint[0] * normal[0] + toPoint[1] * normal[1] + toPoint[2] * normal[2];
  return distance;
}