/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterPolylineTest
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

#include "albaOpImporterPolylineTest.h"
#include "albaOpImporterPolyline.h"

#include "albaString.h"
#include "albaVMEPolyline.h"
#include "albaVMEStorage.h"

#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
static vtkPolyData *GetPolyData(albaVME *vme)
{
	vme->GetOutput()->Update();
	return vtkPolyData::SafeDownCast(vme->GetOutput()->GetVTKData());
}

//----------------------------------------------------------------------------
void albaOpImporterPolylineTest::TestImport()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	albaOpImporterPolyline *importer = new albaOpImporterPolyline("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/Polyline/Polyline.txt";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);

	CPPUNIT_ASSERT(importer->Import() == ALBA_OK);

	albaVMEPolyline *node = albaVMEPolyline::SafeDownCast(importer->GetOutput());
	CPPUNIT_ASSERT(node != NULL);

	vtkPolyData *polyData = GetPolyData(node);
	CPPUNIT_ASSERT(polyData != NULL);

	// Two polylines (3 and 2 points); the single trailing point is not a line
	CPPUNIT_ASSERT(polyData->GetNumberOfPoints() == 6);
	CPPUNIT_ASSERT(polyData->GetLines()->GetNumberOfCells() == 2);
	CPPUNIT_ASSERT(polyData->GetCell(0)->GetNumberOfPoints() == 3);
	CPPUNIT_ASSERT(polyData->GetCell(1)->GetNumberOfPoints() == 2);
	CPPUNIT_ASSERT(polyData->GetPointData()->GetNumberOfArrays() == 0);

	double xyz[3];
	polyData->GetPoint(polyData->GetCell(1)->GetPointId(1), xyz);
	CPPUNIT_ASSERT(fabs(xyz[0] - 6) < 0.001 && fabs(xyz[1] - 5) < 0.001 && fabs(xyz[2] - 5) < 0.001);

	cppDEL(importer);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpImporterPolylineTest::TestScalars()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	albaOpImporterPolyline *importer = new albaOpImporterPolyline("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/Polyline/Polyline.txt";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	importer->SetNumberOfScalars(1);
	importer->SetScalarColumn(0, 4);
	importer->SetScalarName(0, "S1");

	CPPUNIT_ASSERT(importer->Import() == ALBA_OK);

	vtkPolyData *polyData = GetPolyData(importer->GetOutput());
	vtkDataArray *array = polyData->GetPointData()->GetArray("S1");
	CPPUNIT_ASSERT(array != NULL);
	CPPUNIT_ASSERT(array->GetNumberOfTuples() == 6);
	CPPUNIT_ASSERT(fabs(array->GetTuple1(3) - 20) < 0.001);
	CPPUNIT_ASSERT(fabs(array->GetTuple1(5) - 30) < 0.001);

	cppDEL(importer);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpImporterPolylineTest::TestSetters()
{
	albaOpImporterPolyline *importer = new albaOpImporterPolyline("importer");

	CPPUNIT_ASSERT(importer->GetNumberOfScalars() == 0);
	importer->SetNumberOfScalars(7);
	CPPUNIT_ASSERT(importer->GetNumberOfScalars() == 0);
	importer->SetNumberOfScalars(-1);
	CPPUNIT_ASSERT(importer->GetNumberOfScalars() == 0);
	importer->SetNumberOfScalars(3);
	CPPUNIT_ASSERT(importer->GetNumberOfScalars() == 3);

	importer->SetScalarColumn(2, 9);
	CPPUNIT_ASSERT(importer->GetScalarColumn(2) == 9);
	importer->SetScalarColumn(3, 9);
	CPPUNIT_ASSERT(importer->GetScalarColumn(3) == -1);

	importer->SetScalarName(1, "Name");
	CPPUNIT_ASSERT(importer->GetScalarName(1).Equals("Name"));
	CPPUNIT_ASSERT(importer->GetScalarName(3).Equals(""));

	importer->SetFirstCoordColumn(2);
	CPPUNIT_ASSERT(importer->GetFirstCoordColumn() == 2);

	cppDEL(importer);
}

//----------------------------------------------------------------------------
void albaOpImporterPolylineTest::TestWrongColumns()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	albaOpImporterPolyline *importer = new albaOpImporterPolyline("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/Polyline/Polyline.txt";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);

	// The file has only four columns
	importer->SetNumberOfScalars(1);
	importer->SetScalarColumn(0, 5);
	CPPUNIT_ASSERT(importer->Import() == ALBA_ERROR);
	CPPUNIT_ASSERT(importer->GetOutput() == NULL);

	cppDEL(importer);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpImporterPolylineTest::TestCommentLine()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	albaOpImporterPolyline *importer = new albaOpImporterPolyline("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/Polyline/Polyline.txt";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);

	CPPUNIT_ASSERT(importer->GetCommentLine().Equals("#"));

	// With a different comment marker the header lines cannot be parsed
	importer->SetCommentLine("%");
	CPPUNIT_ASSERT(importer->Import() == ALBA_ERROR);

	importer->SetCommentLine("#");
	CPPUNIT_ASSERT(importer->Import() == ALBA_OK);

	cppDEL(importer);
	albaDEL(storage);
}
