/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELandmarkCloudTest
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

#include "albaVMELandmarkCloudTest.h"
#include <wx/dir.h>

#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEOutputPointSet.h"
#include "albaVMEStorage.h"
#include "albaVMEFactory.h"
#include "mmaMaterial.h"
#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSphereSource.h"
//#include "albaMatrix.h"

#include <iostream>
#include <set>

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{
  albaVMELandmarkCloud *cloud;
  albaNEW(cloud);
  albaDEL(cloud);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::AppendLandmarkTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  cloud->TestModeOn();
  cloud->AppendLandmark(x, y, z, name.GetCStr());

  landmark = cloud->GetLandmark(name);

  bool result = false;
  //Check if landmark has been appended
  result = (name.Compare(landmark->GetName()) == 0);
  CPPUNIT_ASSERT(result);

  albaSmartPointer<albaVMELandmarkCloud> cloud1;
  albaSmartPointer<albaVMELandmark> landmark1;

  landmark1->SetName(name);
  landmark1->SetPoint(z,y,z);

  cloud1->TestModeOn();
  cloud1->AppendLandmark(name);

  //Check if landmark has been appended
  result = false;
  result = (name.Compare(cloud1->GetLandmark(name)->GetName()) == 0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetLandmarkTest()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  landmark->SetName(name);
  landmark->SetPoint(z,y,z);

  cloud->TestModeOn();
	landmark->ReparentTo(cloud);

  bool result = false;
  //Check if landmark has been set
  result = (name.Compare(cloud->GetLandmark(name)->GetName()) == 0);
  CPPUNIT_ASSERT(result);

  albaSmartPointer<albaVMELandmarkCloud> cloud1;
  albaSmartPointer<albaVMELandmark> landmark1;

  cloud1->TestModeOn();
  cloud1->AppendLandmark(name);
  cloud1->SetLandmark(name,x,y,z);

  double xyz[3];
  cloud1->GetLandmark(name, xyz);

  //Check if landmark has been set
  result = false;
  result = albaEquals(x,xyz[0]) && albaEquals(y,xyz[1]) && albaEquals(z,xyz[2]);
  CPPUNIT_ASSERT(result);

  albaSmartPointer<albaVMELandmarkCloud> cloud2;
  albaSmartPointer<albaVMELandmark> landmark2;

  cloud2->TestModeOn();
  cloud2->AppendLandmark(name);
  cloud2->SetLandmark(0,x,y,z);

  double xyz1[3];
  cloud2->GetLandmarkPosition(0,xyz1);

  //Check if landmark has been set
  result = false;
  result = albaEquals(x,xyz1[0]) && albaEquals(y,xyz1[1]) && albaEquals(z,xyz1[2]);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::FindLandmarkIndexTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud2;
  albaSmartPointer<albaVMELandmark> landmark2;

  cloud2->TestModeOn();
  cloud2->AppendLandmark(name);
  cloud2->SetLandmark(0,x,y,z);

  //FindLandmarkIndex
  bool result = false;
  result = albaEquals(cloud2->GetLandmarkIndex(name),0);
  CPPUNIT_ASSERT(result);
}


//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::RemoveLandmarkTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);

  cloud->RemoveLandmark(0);

  //Check if landmark has been removed
  bool result = false;
  result = albaEquals(cloud->GetNumberOfChildren(),0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetNumberOfLandmarksTest()
//----------------------------------------------------------------------------
{
  int number;

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->SetNumberOfLandmarks(10);
  number = cloud->GetNumberOfLandmarks();

  //Check if landmark number is correct
  bool result = false;
  result = albaEquals(number,10);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::GetLandmarkNameTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark("");
  cloud->SetLandmark(0,x,y,z);
  cloud->SetLandmarkName(0, name);

  albaString nameRet = cloud->GetLandmarkName(0);

  //Check if landmark name is correct
  bool result = false;
  result = (nameRet.Compare(name) == 0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetRadiusTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  double radius = 6;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);
  cloud->SetRadius(radius);

  //Check if landmark radius has been set
  bool result = false;
  result = albaEquals(cloud->GetRadius(),radius);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetSphereResolutionTest()
//----------------------------------------------------------------------------
{
  double x = 5;
  double y = 10;
  double z = 15.5;
  double res = 30;
  albaString name = "land";

  albaSmartPointer<albaVMELandmarkCloud> cloud;
  albaSmartPointer<albaVMELandmark> landmark;

  cloud->TestModeOn();
  cloud->AppendLandmark(name);
  cloud->SetLandmark(0,x,y,z);
  cloud->SetSphereResolution(res);

  //Check if landmark resolution has been set
  bool result = false;
  result = albaEquals(cloud->GetSphereResolution(),res);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
	albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
	albaSmartPointer<albaVMELandmark> landmark1;
	albaSmartPointer<albaVMELandmark> landmark2;

  albaSmartPointer<albaVMELandmarkCloud> newLandmarkCloud;
 
  double rad = 10;
  double res = 30;

	landmark1->SetName("1");
	landmark2->SetName("2");

  landmarkCloud->TestModeOn();
  landmarkCloud->SetRadius(rad);
  landmarkCloud->SetSphereResolution(res);
	landmark1->ReparentTo(landmarkCloud);
	landmark2->ReparentTo(landmarkCloud);


  newLandmarkCloud->DeepCopy(landmarkCloud);
  
  //Check if landmark cloud has been copied
  bool result = false;
  result = albaEquals(newLandmarkCloud->GetRadius(),rad);
  CPPUNIT_ASSERT(result);

  result = false;
  result = albaEquals(newLandmarkCloud->GetSphereResolution(),res);
  CPPUNIT_ASSERT(result);

  result = false;
  result = albaEquals(newLandmarkCloud->GetNumberOfLandmarks(),0);
  CPPUNIT_ASSERT(result);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::CopyTreeTest()
{
	albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;
	albaSmartPointer<albaVMELandmark> landmark1;
	albaSmartPointer<albaVMELandmark> landmark2;

	double rad = 10;
	double res = 30;

	landmark1->SetName("1");
	landmark2->SetName("2");

	landmarkCloud->TestModeOn();
	landmarkCloud->SetRadius(rad);
	landmarkCloud->SetSphereResolution(res);
	landmark1->ReparentTo(landmarkCloud);
	landmark2->ReparentTo(landmarkCloud);

	albaVMELandmarkCloud *newLandmarkCloud = (albaVMELandmarkCloud *)landmarkCloud->CopyTree();

	//Check if landmark cloud has been copied
	bool result = false;
	result = albaEquals(newLandmarkCloud->GetRadius(), rad);
	CPPUNIT_ASSERT(result);

	result = false;
	result = albaEquals(newLandmarkCloud->GetSphereResolution(), res);
	CPPUNIT_ASSERT(result);

	result = false;
	result = albaEquals(newLandmarkCloud->GetNumberOfLandmarks(), landmarkCloud->GetNumberOfLandmarks());
	CPPUNIT_ASSERT(result);
}

#define NUM_OF_POINTS 5

//----------------------------------------------------------------------------
static bool PointEquals(double *a, double x, double y, double z)
{
	return albaEquals(a[0], x) && albaEquals(a[1], y) && albaEquals(a[2], z);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::SetDataPointsOnlyTest()
{
	albaSmartPointer<albaVMELandmarkCloud> cloud;
	cloud->TestModeOn();

	vtkALBASmartPointer<vtkPolyData> polydata;
	vtkALBASmartPointer<vtkPoints> pts;
	for (int i = 0; i < NUM_OF_POINTS; i++)
		pts->InsertNextPoint(i, i, i);
	polydata->SetPoints(pts);

	// SetData is private in albaVMELandmarkCloud, it is called through the base class like the importers do
	albaVMEGeneric *generic = cloud.GetPointer();
	CPPUNIT_ASSERT(generic->SetData(polydata, 0.0) == ALBA_OK);

	// polydata with lines are not accepted
	vtkALBASmartPointer<vtkCellArray> cells;
	for (int i = 0; i < NUM_OF_POINTS - 1; i++)
	{
		vtkIdType cell[2] = { i, i + 1 };
		cells->InsertNextCell(2, cell);
	}
	polydata->SetLines(cells);
	CPPUNIT_ASSERT(generic->SetData(polydata, 0.0) == ALBA_ERROR);

	// polydata with polygons are not accepted
	vtkALBASmartPointer<vtkSphereSource> sphere;
	sphere->Update();
	CPPUNIT_ASSERT(generic->SetData(sphere->GetOutput(), 0.0) == ALBA_ERROR);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::CreateLMStructureFromDataVectorTest()
{
	albaSmartPointer<albaVMELandmarkCloud> cloud;
	cloud->TestModeOn();

	vtkALBASmartPointer<vtkPolyData> polydata;
	vtkALBASmartPointer<vtkPoints> pts;
	for (int i = 0; i < NUM_OF_POINTS; i++)
		pts->InsertNextPoint(i, 2 * i, 3 * i);
	polydata->SetPoints(pts);

	// same steps of the VTK importer
	albaVMEGeneric *generic = cloud.GetPointer();
	CPPUNIT_ASSERT(generic->SetDataByDetaching(polydata, 0.0) == ALBA_OK);
	cloud->CreateLMStructureFromDataVector();

	CPPUNIT_ASSERT(cloud->GetNumberOfLandmarks() == NUM_OF_POINTS);
	CPPUNIT_ASSERT(cloud->GetNumberOfChildren() == NUM_OF_POINTS);
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(0.0) == NUM_OF_POINTS);

	for (int i = 0; i < NUM_OF_POINTS; i++)
	{
		double xyz[3];
		albaString name;
		name << "Landmark " << i;
		CPPUNIT_ASSERT(name.Equals(cloud->GetLandmarkName(i)));

		cloud->GetLandmark(i, xyz);
		CPPUNIT_ASSERT(PointEquals(xyz, i, 2 * i, 3 * i));

		CPPUNIT_ASSERT(cloud->GetPoint(i, xyz, 0.0) == ALBA_OK);
		CPPUNIT_ASSERT(PointEquals(xyz, i, 2 * i, 3 * i));
	}
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::PolydataPointsTest()
{
	double pos[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
	const char *names[3] = { "lm0", "lm1", "lm2" };

	albaSmartPointer<albaVMELandmarkCloud> cloud;
	cloud->TestModeOn();
	for (int i = 0; i < 3; i++)
		CPPUNIT_ASSERT(cloud->AppendLandmark(pos[i], names[i]) == i);

	CPPUNIT_ASSERT(cloud->GetNumberOfLandmarks() == 3);
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(0.0) == 3);
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints() == 3);

	cloud->GetOutput()->Update();
	vtkPolyData *output = vtkPolyData::SafeDownCast(cloud->GetOutput()->GetVTKData());
	CPPUNIT_ASSERT(output != NULL);
	CPPUNIT_ASSERT(output->GetNumberOfPoints() == 3);

	for (int i = 0; i < 3; i++)
	{
		double xyz[3], x, y, z;
		CPPUNIT_ASSERT(cloud->GetPoint(i, xyz, 0.0) == ALBA_OK);
		CPPUNIT_ASSERT(PointEquals(xyz, pos[i][0], pos[i][1], pos[i][2]));

		CPPUNIT_ASSERT(cloud->GetPoint(i, x, y, z, 0.0) == ALBA_OK);
		CPPUNIT_ASSERT(albaEquals(x, pos[i][0]) && albaEquals(y, pos[i][1]) && albaEquals(z, pos[i][2]));

		cloud->GetLandmarkPosition(i, xyz, 0.0);
		CPPUNIT_ASSERT(PointEquals(xyz, pos[i][0], pos[i][1], pos[i][2]));

		output->GetPoint(i, xyz);
		CPPUNIT_ASSERT(PointEquals(xyz, pos[i][0], pos[i][1], pos[i][2]));
	}

	// moving a landmark updates the point
	CPPUNIT_ASSERT(cloud->SetLandmark(2, 10, 11, 12) == ALBA_OK);
	double xyz[3];
	CPPUNIT_ASSERT(cloud->GetPoint(2, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 10, 11, 12));
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(0.0) == 3);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::TimeVaryingPointsTest()
{
	albaSmartPointer<albaVMELandmarkCloud> cloud;
	cloud->TestModeOn();
	cloud->AppendLandmark(1, 2, 3, "lm0");
	cloud->AppendLandmark(4, 5, 6, "lm1");

	CPPUNIT_ASSERT(cloud->IsRigid());

	// setting a landmark in a new time stamp creates a new item with the same number of points
	CPPUNIT_ASSERT(cloud->SetLandmark(0, 10, 20, 30, 1.0) == ALBA_OK);

	CPPUNIT_ASSERT(!cloud->IsRigid());
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(0.0) == 2);
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(1.0) == 2);

	double xyz[3];
	CPPUNIT_ASSERT(cloud->GetPoint(0, xyz, 1.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 10, 20, 30));

	// the other points of the new item are created in the origin
	CPPUNIT_ASSERT(cloud->GetPoint(1, xyz, 1.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 0, 0, 0));

	// the item at time 0 is unchanged
	CPPUNIT_ASSERT(cloud->GetPoint(0, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 1, 2, 3));
	CPPUNIT_ASSERT(cloud->GetPoint(1, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 4, 5, 6));

	// a new landmark is added to all the items
	cloud->AppendLandmark(7, 8, 9, "lm2");
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(0.0) == 3);
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(1.0) == 3);
	CPPUNIT_ASSERT(cloud->GetPoint(2, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 7, 8, 9));
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::RemoveLandmarkPointsTest()
{
	albaSmartPointer<albaVMELandmarkCloud> cloud;
	cloud->TestModeOn();
	cloud->AppendLandmark(1, 2, 3, "lm0");
	cloud->AppendLandmark(4, 5, 6, "lm1");
	cloud->AppendLandmark(7, 8, 9, "lm2");
	cloud->SetLandmark(1, 40, 50, 60, 1.0);

	// the point is removed from all the items and the following points are shifted
	CPPUNIT_ASSERT(cloud->RemoveLandmark(0) == ALBA_OK);

	CPPUNIT_ASSERT(cloud->GetNumberOfLandmarks() == 2);
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(0.0) == 2);
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(1.0) == 2);
	CPPUNIT_ASSERT(albaString("lm1").Equals(cloud->GetLandmarkName(0)));
	CPPUNIT_ASSERT(albaString("lm2").Equals(cloud->GetLandmarkName(1)));

	double xyz[3];
	CPPUNIT_ASSERT(cloud->GetPoint(0, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 4, 5, 6));
	CPPUNIT_ASSERT(cloud->GetPoint(1, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 7, 8, 9));
	CPPUNIT_ASSERT(cloud->GetPoint(0, xyz, 1.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 40, 50, 60));

	// removing the last one
	CPPUNIT_ASSERT(cloud->RemoveLandmark(1) == ALBA_OK);
	CPPUNIT_ASSERT(cloud->GetNumberOfPoints(0.0) == 1);
	CPPUNIT_ASSERT(cloud->GetPoint(0, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 4, 5, 6));
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::ShowLandmarkTest()
{
	albaSmartPointer<albaVMELandmarkCloud> cloud;
	cloud->TestModeOn();
	cloud->AppendLandmark(1, 2, 3, "lm0");
	cloud->AppendLandmark(4, 5, 6, "lm1");
	albaVMELandmark *lm0 = cloud->GetLandmark(0);
	albaVMELandmark *lm1 = cloud->GetLandmark(1);

	// appended landmarks are not shown: the points have no vertex cells
	cloud->GetOutput()->Update();
	vtkPolyData *output = vtkPolyData::SafeDownCast(cloud->GetOutput()->GetVTKData());
	CPPUNIT_ASSERT(output->GetNumberOfPoints() == 2);
	CPPUNIT_ASSERT(output->GetNumberOfVerts() == 0);
	CPPUNIT_ASSERT(cloud->GetLandmarkShowNumber() == 0);

	cloud->ShowLandmark(lm0, true);
	output = vtkPolyData::SafeDownCast(cloud->GetOutput()->GetVTKData());
	CPPUNIT_ASSERT(output->GetNumberOfVerts() == 1);
	CPPUNIT_ASSERT(cloud->IsLandmarkShow(lm0));
	CPPUNIT_ASSERT(!cloud->IsLandmarkShow(lm1));
	CPPUNIT_ASSERT(cloud->GetLandmarkShowNumber() == 1);

	cloud->ShowAllLandmarks(true);
	output = vtkPolyData::SafeDownCast(cloud->GetOutput()->GetVTKData());
	CPPUNIT_ASSERT(output->GetNumberOfVerts() == 2);
	CPPUNIT_ASSERT(cloud->GetLandmarkShowNumber() == 2);

	cloud->ShowLandmark(lm0, false);
	output = vtkPolyData::SafeDownCast(cloud->GetOutput()->GetVTKData());
	CPPUNIT_ASSERT(output->GetNumberOfVerts() == 1);
	CPPUNIT_ASSERT(!cloud->IsLandmarkShow(lm0));
	CPPUNIT_ASSERT(cloud->IsLandmarkShow(lm1));
	CPPUNIT_ASSERT(cloud->GetLandmarkShowNumber() == 1);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::OutputPointSetTest()
{
	albaSmartPointer<albaVMELandmarkCloud> cloud;
	cloud->TestModeOn();
	cloud->AppendLandmark(1, 2, 3, "lm0");
	cloud->AppendLandmark(4, 5, 6, "lm1");

	CPPUNIT_ASSERT(cloud->GetOutput()->IsA("albaVMEOutputLandmarkCloud"));
	CPPUNIT_ASSERT(cloud->GetOutput()->IsALBAType(albaVMEOutputPointSet));

	albaVMEOutputPointSet *output = albaVMEOutputPointSet::SafeDownCast(cloud->GetOutput());
	output->Update();
	vtkPolyData *data = output->GetPointSetData();
	CPPUNIT_ASSERT(data != NULL);
	CPPUNIT_ASSERT(data->GetNumberOfPoints() == 2);

	double xyz[3];
	data->GetPoint(1, xyz);
	CPPUNIT_ASSERT(PointEquals(xyz, 4, 5, 6));

	// the material is shared between the VME and its output
	mmaMaterial *material = cloud->GetMaterial();
	CPPUNIT_ASSERT(material != NULL);
	CPPUNIT_ASSERT(output->GetMaterial() == material);
}

//----------------------------------------------------------------------------
void albaVMELandmarkCloudTest::StoreRestoreTest()
{
	albaString fileName = GET_TEST_DATA_DIR();
	fileName << "\\landmarkCloudStoreRestore.msf";

	albaVMEStorage *storage = albaVMEStorage::New();
	storage->SetURL(fileName.GetCStr());
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	albaVMELandmarkCloud *cloud;
	albaNEW(cloud);
	cloud->TestModeOn();
	cloud->SetName("cloud");
	storage->GetRoot()->AddChild(cloud);
	cloud->AppendLandmark(1, 2, 3, "lm0");
	cloud->AppendLandmark(4, 5, 6, "lm1");
	cloud->AppendLandmark(7, 8, 9, "lm2");
	cloud->SetLandmark(1, 40, 50, 60, 1.0);
	cloud->SetRadius(3);
	cloud->SetSphereResolution(20);
	cloud->ShowLandmark(cloud->GetLandmark(0), true);

	CPPUNIT_ASSERT(storage->Store() == ALBA_OK);

	storage->GetRoot()->CleanTree();
	albaDEL(cloud);
	albaDEL(storage);

	// restore the tree in a new storage
	albaVMEFactory::Initialize();
	albaVMEStorage *storage2 = albaVMEStorage::New();
	storage2->SetURL(fileName.GetCStr());
	CPPUNIT_ASSERT(storage2->Restore() == ALBA_OK);

	albaVMERoot *root = storage2->GetRoot();
	CPPUNIT_ASSERT(root->GetNumberOfChildren() == 1);
	albaVMELandmarkCloud *restored = albaVMELandmarkCloud::SafeDownCast(root->GetChild(0));
	CPPUNIT_ASSERT(restored != NULL);
	restored->TestModeOn();

	CPPUNIT_ASSERT(restored->GetNumberOfLandmarks() == 3);
	CPPUNIT_ASSERT(albaString("lm0").Equals(restored->GetLandmarkName(0)));
	CPPUNIT_ASSERT(albaString("lm1").Equals(restored->GetLandmarkName(1)));
	CPPUNIT_ASSERT(albaString("lm2").Equals(restored->GetLandmarkName(2)));
	CPPUNIT_ASSERT(albaEquals(restored->GetRadius(), 3));
	CPPUNIT_ASSERT(restored->GetSphereResolution() == 20);
	CPPUNIT_ASSERT(restored->GetNumberOfPoints(0.0) == 3);
	CPPUNIT_ASSERT(restored->GetNumberOfPoints(1.0) == 3);

	double xyz[3];
	CPPUNIT_ASSERT(restored->GetPoint(0, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 1, 2, 3));
	CPPUNIT_ASSERT(restored->GetPoint(2, xyz, 0.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 7, 8, 9));
	CPPUNIT_ASSERT(restored->GetPoint(1, xyz, 1.0) == ALBA_OK);
	CPPUNIT_ASSERT(PointEquals(xyz, 40, 50, 60));

	root->CleanTree();
	albaDEL(storage2);
}
