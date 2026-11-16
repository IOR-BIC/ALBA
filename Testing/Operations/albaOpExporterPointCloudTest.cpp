/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterPointCloudTest
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

#include "albaOpExporterPointCloudTest.h"
#include "albaOpExporterPointCloud.h"
#include "albaOpImporterPointCloud.h"

#include "albaString.h"
#include "albaVMEPointCloud.h"
#include "albaVMEPolyline.h"
#include "albaVMESurface.h"
#include "albaVMEStorage.h"

#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

#include <string.h>

//----------------------------------------------------------------------------
void albaOpExporterPointCloudTest::TestAccept()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	albaVMEPointCloud *pointCloud;
	albaNEW(pointCloud);
	albaVMESurface *surface;
	albaNEW(surface);
	albaVMEPolyline *polyline;
	albaNEW(polyline);

	albaOpExporterPointCloud *exporter = new albaOpExporterPointCloud();
	CPPUNIT_ASSERT(exporter->Accept(pointCloud));
	CPPUNIT_ASSERT(!exporter->Accept(surface));
	CPPUNIT_ASSERT(!exporter->Accept(polyline));
	CPPUNIT_ASSERT(!exporter->Accept(root));
	CPPUNIT_ASSERT(!exporter->Accept(NULL));

	cppDEL(exporter);
	albaDEL(polyline);
	albaDEL(surface);
	albaDEL(pointCloud);
	albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaOpExporterPointCloudTest::TestExport()
{
	albaVMEStorage *storage = albaVMEStorage::New();
	albaVME *root = (albaVME *)storage->GetRoot();

	// Import the reference point cloud with one scalar (column 4, "ID1")
	albaOpImporterPointCloud *importer = new albaOpImporterPointCloud("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/PointCloud/PointCloud.dat";
	importer->SetFileName(filename.GetCStr());
	importer->SetInput(root);
	CPPUNIT_ASSERT(importer->Import() == ALBA_OK);

	albaVME *imported = importer->GetOutput();
	CPPUNIT_ASSERT(imported != NULL);

	// Export it
	albaString exportedFile = GET_TEST_DATA_DIR();
	exportedFile << "\\exported_PointCloud.txt";

	albaOpExporterPointCloud *exporter = new albaOpExporterPointCloud();
	exporter->TestModeOn();
	exporter->SetInput(imported);
	exporter->SetFileName(exportedFile.GetCStr());
	CPPUNIT_ASSERT(exporter->ExportPointCloud());

	// Check the header and the number of rows
	FILE *file = fopen(exportedFile.GetCStr(), "r");
	CPPUNIT_ASSERT(file != NULL);

	char line[1024];
	CPPUNIT_ASSERT(fgets(line, sizeof(line), file) != NULL);
	CPPUNIT_ASSERT(strcmp(line, "#\tX\tY\tZ\tID1\n") == 0);

	int rows = 0;
	while (fgets(line, sizeof(line), file) != NULL)
		rows++;
	fclose(file);
	CPPUNIT_ASSERT(rows == 30);

	// Import the exported file again: coordinates are in columns 1-3, the scalar in column 4
	albaOpImporterPointCloud *reimporter = new albaOpImporterPointCloud("reimporter");
	reimporter->TestModeOn();
	reimporter->SetFileName(exportedFile.GetCStr());
	reimporter->SetInput(root);
	reimporter->SetCommentLine("#");
	CPPUNIT_ASSERT(reimporter->Import() == ALBA_OK);

	albaVMEPointCloud *reimported = albaVMEPointCloud::SafeDownCast(reimporter->GetOutput());
	CPPUNIT_ASSERT(reimported != NULL);
	CPPUNIT_ASSERT(reimported->GetNumberOfPoints() == 30);

	double xyz[3];
	reimported->GetPoint(19, xyz);
	CPPUNIT_ASSERT(fabs(xyz[0] - 20) < 0.01 && fabs(xyz[1] - 9) < 0.01 && fabs(xyz[2] - -12.349327) < 0.01);

	reimported->GetOutput()->Update();
	vtkPolyData *polyData = vtkPolyData::SafeDownCast(reimported->GetOutput()->GetVTKData());
	vtkDataArray *array = polyData->GetPointData()->GetArray("ID1");
	CPPUNIT_ASSERT(array != NULL);
	CPPUNIT_ASSERT(fabs(array->GetTuple1(19) - 8.346818) < 0.01);

	cppDEL(reimporter);
	cppDEL(exporter);
	cppDEL(importer);
	albaDEL(storage);
}
