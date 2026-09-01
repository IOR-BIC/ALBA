/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterPointCloudTest
 Authors: Gianluigi Crimi
 
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

#include "albaOpImporterPointCloudTest.h"
#include "albaOpImporterPointCloud.h"


#include "albaString.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEPointCloud.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"

#include <string>
#include <assert.h>
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

//-----------------------------------------------------------
void albaOpImporterPointCloudTest::TestImport() 
//-----------------------------------------------------------
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();


	albaOpImporterPointCloud *importer=new albaOpImporterPointCloud("importer");
	importer->TestModeOn();

	albaString filename=ALBA_DATA_ROOT;
  filename<<"/PointCloud/PointCloud.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	importer->Import();
	albaVMEPointCloud *node=(albaVMEPointCloud *)importer->GetOutput();

  CPPUNIT_ASSERT(node->GetNumberOfPoints() == 30);

  double xyz[3];
	node->GetPoint(19, xyz);

  
	double x = 20;
  double y = 9;
  double z = -12.349327;

  CPPUNIT_ASSERT( fabs(xyz[0] - x) < 0.01 &&
                  fabs(xyz[1] - y) < 0.01 &&
                  fabs(xyz[2] - z) < 0.01 );

	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());

	double scalarValue = polyData->GetPointData()->GetScalars("ID1")->GetTuple1(19);

	CPPUNIT_ASSERT(fabs(scalarValue - 8.346818) < 0.01);

  cppDEL(importer);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloudTest::TestFirstCoordCol()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();


	albaOpImporterPointCloud *importer = new albaOpImporterPointCloud("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/PointCloud/PointCloud.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	
	importer->SetFirstCoordColumn(3);
	importer->SetScalarColumn(0, 6);

	importer->Import();
	albaVMEPointCloud *node = (albaVMEPointCloud *)importer->GetOutput();

	CPPUNIT_ASSERT(node->GetNumberOfPoints() == 30);

	double xyz[3];
	node->GetPoint(19, xyz);


	double x = -12.349327;
	double y = 8.346818;
	double z = 0.964494;

	CPPUNIT_ASSERT(fabs(xyz[0] - x) < 0.01 &&
		fabs(xyz[1] - y) < 0.01 &&
		fabs(xyz[2] - z) < 0.01);

	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());

	double scalarValue = polyData->GetPointData()->GetScalars("ID1")->GetTuple1(19);

	CPPUNIT_ASSERT(fabs(scalarValue - -12.312758) < 0.01);

	cppDEL(importer);
	albaDEL(storage);

}

//----------------------------------------------------------------------------
void albaOpImporterPointCloudTest::TestNumberOfScalars()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();


	albaOpImporterPointCloud *importer = new albaOpImporterPointCloud("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/PointCloud/PointCloud.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);

	//Test Upper Bound
	importer->SetNumberOfScalars(7);
	CPPUNIT_ASSERT(importer->GetNumberOfScalars() == 1);

	//Test Lower Bound
	importer->SetNumberOfScalars(-1);
	CPPUNIT_ASSERT(importer->GetNumberOfScalars() == 1);

	//Test Lower Bound
	importer->SetNumberOfScalars(3);
	CPPUNIT_ASSERT(importer->GetNumberOfScalars() == 3);

	importer->Import();
	albaVMEPointCloud *node = (albaVMEPointCloud *)importer->GetOutput();

	CPPUNIT_ASSERT(node->GetNumberOfPoints() == 30);

	double xyz[3];
	node->GetPoint(19, xyz);


	double x = 20;
	double y = 9;
	double z = -12.349327;

	CPPUNIT_ASSERT(fabs(xyz[0] - x) < 0.01 &&
		fabs(xyz[1] - y) < 0.01 &&
		fabs(xyz[2] - z) < 0.01);

	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());

	double scalarValue = polyData->GetPointData()->GetScalars("ID1")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - 8.346818) < 0.01);

	scalarValue = polyData->GetPointData()->GetScalars("ID2")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - 0.964494) < 0.01);
	
	scalarValue = polyData->GetPointData()->GetScalars("ID3")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - -12.312758) < 0.01);

	cppDEL(importer);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloudTest::TestScalarColumn()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();


	albaOpImporterPointCloud *importer = new albaOpImporterPointCloud("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/PointCloud/PointCloud.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	importer->SetNumberOfScalars(3);

	importer->SetScalarColumn(0, 11);
	CPPUNIT_ASSERT(importer->GetScalarColumn(0) == 11);

	importer->SetScalarColumn(1, 4);
	CPPUNIT_ASSERT(importer->GetScalarColumn(1) == 4);

	importer->SetScalarColumn(2, 6);
	CPPUNIT_ASSERT(importer->GetScalarColumn(2) == 6);

	importer->Import();
	albaVMEPointCloud *node = (albaVMEPointCloud *)importer->GetOutput();

	CPPUNIT_ASSERT(node->GetNumberOfPoints() == 30);

	double xyz[3];
	node->GetPoint(19, xyz);


	double x = 20;
	double y = 9;
	double z = -12.349327;

	CPPUNIT_ASSERT(fabs(xyz[0] - x) < 0.01 &&
		fabs(xyz[1] - y) < 0.01 &&
		fabs(xyz[2] - z) < 0.01);

	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());

	double scalarValue = polyData->GetPointData()->GetScalars("ID1")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - 0.126466) < 0.01);

	scalarValue = polyData->GetPointData()->GetScalars("ID2")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - 8.346908) < 0.01);

	scalarValue = polyData->GetPointData()->GetScalars("ID3")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - -12.312758) < 0.01);

	cppDEL(importer);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloudTest::TestScalarNames()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();


	albaOpImporterPointCloud *importer = new albaOpImporterPointCloud("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/PointCloud/PointCloud.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	importer->SetNumberOfScalars(3);

	importer->SetScalarName(0, "1_ID");
	CPPUNIT_ASSERT(importer->GetScalarName(0).Equals("1_ID"));
	importer->SetScalarName(1, "2_ID");
	CPPUNIT_ASSERT(importer->GetScalarName(1).Equals("2_ID"));
	importer->SetScalarName(2, "3_ID");
	CPPUNIT_ASSERT(importer->GetScalarName(2).Equals("3_ID"));

	
	importer->Import();
	albaVMEPointCloud *node = (albaVMEPointCloud *)importer->GetOutput();

	CPPUNIT_ASSERT(node->GetNumberOfPoints() == 30);

	double xyz[3];
	node->GetPoint(19, xyz);


	double x = 20;
	double y = 9;
	double z = -12.349327;

	CPPUNIT_ASSERT(fabs(xyz[0] - x) < 0.01 &&
		fabs(xyz[1] - y) < 0.01 &&
		fabs(xyz[2] - z) < 0.01);

	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());

	double scalarValue = polyData->GetPointData()->GetScalars("1_ID")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - 8.346818) < 0.01);

	scalarValue = polyData->GetPointData()->GetScalars("2_ID")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - 0.964494) < 0.01);

	scalarValue = polyData->GetPointData()->GetScalars("3_ID")->GetTuple1(19);
	CPPUNIT_ASSERT(fabs(scalarValue - -12.312758) < 0.01);

	cppDEL(importer);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpImporterPointCloudTest::TestCommentLine()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();


	albaOpImporterPointCloud *importer = new albaOpImporterPointCloud("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/PointCloud/pointCloudComments.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);

	int importResult=importer->Import();
	CPPUNIT_ASSERT(importResult == ALBA_ERROR);

	importer->SetCommentLine("//");

	CPPUNIT_ASSERT(importer->GetCommentLine().Equals("//"));

	importResult = importer->Import();
	CPPUNIT_ASSERT(importResult == ALBA_OK);


	albaVMEPointCloud *node = (albaVMEPointCloud *)importer->GetOutput();

	CPPUNIT_ASSERT(node->GetNumberOfPoints() == 30);

	double xyz[3];
	node->GetPoint(19, xyz);


	double x = 20;
	double y = 9;
	double z = -12.349327;

	CPPUNIT_ASSERT(fabs(xyz[0] - x) < 0.01 &&
		fabs(xyz[1] - y) < 0.01 &&
		fabs(xyz[2] - z) < 0.01);

	node->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(node->GetOutput()->GetVTKData());

	double scalarValue = polyData->GetPointData()->GetScalars("ID1")->GetTuple1(19);

	CPPUNIT_ASSERT(fabs(scalarValue - 8.346818) < 0.01);

	cppDEL(importer);
	albaDEL(storage);
}
