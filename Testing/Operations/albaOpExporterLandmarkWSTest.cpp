/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterLandmarkWSTest
 Authors: Simone Brazzale
 
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
#pragma runtime_checks( "s", off )
#include "albaOpExporterLandmarkWSTest.h"
#include "albaOpExporterLandmarkWS.h"
#include "albaOpImporterLandmarkWS.h"
#include "albaOpImporterLandmark.h"

#include "albaSmartPointer.h"
#include "albaString.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"

#include <string>
#include <assert.h>

//------------------------------------------------------------------------
void albaOpExporterLandmarkWSTest::TestOnLandmarkImporter()
//------------------------------------------------------------------------
{
	int returnValue = -1;

	// Import input file
	albaOpImporterLandmark *importer = new albaOpImporterLandmark("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
	albaVMELandmarkCloud *node = (albaVMELandmarkCloud *)importer->GetOutput();

	// Initialize and execute exporter
	albaOpExporterLandmarkWS *exporter = new albaOpExporterLandmarkWS("test exporter");
	exporter->SetInput(node);
	albaString fileExp = GET_TEST_DATA_DIR();
	fileExp << "/ExportWS.csv";
	exporter->TestModeOn();
	exporter->SetFileName(fileExp);
	exporter->Write();

	// Import output file
	albaOpImporterLandmarkWS *importerWS = new albaOpImporterLandmarkWS("importer");
	importerWS->TestModeOn();

	importerWS->SetFileName(fileExp.GetCStr());
	importerWS->Read();
	albaVMELandmarkCloud *node_WS = (albaVMELandmarkCloud *)importerWS->GetOutput();

	int result = ALBA_OK;
	std::vector<double *> coord;
	std::vector<double *> coord_WS;

	int numberOfLandmarks = ((albaVMELandmarkCloud *)node)->GetNumberOfLandmarks();
	for (int j = 0; j < numberOfLandmarks; j++)
	{
		albaVMELandmark *landmark = ((albaVMELandmarkCloud *)node)->GetLandmark(j);
		double *xyz = new double[3];
		double rot[3];
		landmark->GetOutput()->GetPose(xyz, rot, 0);
		coord.push_back(xyz);
	}

	numberOfLandmarks = ((albaVMELandmarkCloud *)node_WS)->GetNumberOfLandmarks();
	for (int j = 0; j < numberOfLandmarks; j++)
	{
		albaVMELandmark *landmark = ((albaVMELandmark *)((albaVMELandmarkCloud *)node_WS)->GetLandmark(j));
		double *xyz = new double[3];
		double rot[3];
		landmark->GetOutput()->GetPose(xyz, rot, 0);
		coord_WS.push_back(xyz);
	}

	CPPUNIT_ASSERT(coord.size() == coord_WS.size());

	for (int i = 0; i < coord.size(); i++)
	{
		if (abs(coord[i][0] - coord_WS[i][0]) > 0.05)
			result = 1;
		if (abs(coord[i][1] - coord_WS[i][1]) > 0.05)
			result = 1;
		if (abs(coord[i][2] - coord_WS[i][2]) > 0.05)
			result = 1;
	}

	CPPUNIT_ASSERT(result == ALBA_OK);

	for (int i = 0; i < coord.size(); i++)
	{
		delete coord[i];
	}
	for (int i = 0; i < coord_WS.size(); i++)
	{
		delete coord_WS[i];
	}

	coord.clear();
	coord_WS.clear();

	delete exporter;
	delete importer;
	delete importerWS;
	exporter = NULL;
	importer = NULL;
	importerWS = NULL;
}