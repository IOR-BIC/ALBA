/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterPolylineTest
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

#include "albaOpExporterPolylineTest.h"
#include "albaOpExporterPolyline.h"
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
void albaOpExporterPolylineTest::TestAccept()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	albaVMEPolyline *polyline;
	albaNEW(polyline);

	albaOpExporterPolyline *exporter = new albaOpExporterPolyline();
	CPPUNIT_ASSERT(exporter->Accept(polyline));
	CPPUNIT_ASSERT(!exporter->Accept(root));
	CPPUNIT_ASSERT(!exporter->Accept(NULL));

	cppDEL(exporter);
	albaDEL(polyline);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpExporterPolylineTest::TestExport()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	// Import the reference polyline with its scalar column
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

	albaVME *imported = importer->GetOutput();
	CPPUNIT_ASSERT(imported != NULL);

	// Export it
	albaString exportedFile = GET_TEST_DATA_DIR();
	exportedFile << "\\exported_Polyline.txt";

	albaOpExporterPolyline *exporter = new albaOpExporterPolyline();
	exporter->TestModeOn();
	exporter->SetInput(imported);
	exporter->SetFileName(exportedFile.GetCStr());
	CPPUNIT_ASSERT(exporter->ExportPolyline());

	// Import the exported file again: the point outside any line is not exported
	albaOpImporterPolyline *reimporter = new albaOpImporterPolyline("reimporter");
	reimporter->TestModeOn();
	reimporter->SetFileName(exportedFile.GetCStr());
	reimporter->SetInput(root);
	reimporter->SetNumberOfScalars(1);
	reimporter->SetScalarColumn(0, 4);
	reimporter->SetScalarName(0, "S1");
	CPPUNIT_ASSERT(reimporter->Import() == ALBA_OK);

	albaVME *reimported = reimporter->GetOutput();
	CPPUNIT_ASSERT(reimported != NULL);
	reimported->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(reimported->GetOutput()->GetVTKData());
	CPPUNIT_ASSERT(polyData != NULL);

	CPPUNIT_ASSERT(polyData->GetNumberOfPoints() == 5);
	CPPUNIT_ASSERT(polyData->GetLines()->GetNumberOfCells() == 2);
	CPPUNIT_ASSERT(polyData->GetCell(0)->GetNumberOfPoints() == 3);
	CPPUNIT_ASSERT(polyData->GetCell(1)->GetNumberOfPoints() == 2);

	double xyz[3];
	polyData->GetPoint(2, xyz);
	CPPUNIT_ASSERT(fabs(xyz[0] - 1) < 0.001 && fabs(xyz[1] - 1) < 0.001 && fabs(xyz[2]) < 0.001);

	vtkDataArray *array = polyData->GetPointData()->GetArray("S1");
	CPPUNIT_ASSERT(array != NULL);
	CPPUNIT_ASSERT(fabs(array->GetTuple1(4) - 21) < 0.001);

	cppDEL(reimporter);
	cppDEL(exporter);
	cppDEL(importer);
	albaDEL(storage);
}
