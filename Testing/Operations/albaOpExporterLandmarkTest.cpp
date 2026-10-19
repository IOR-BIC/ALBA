/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterLandmarkTest
 Authors: Daniele Giunchi, Simone Brazzale
 
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
#include "albaOpExporterLandmarkTest.h"
#include "albaOpExporterLandmark.h"
#include "albaOpImporterLandmark.h"

#include "albaSmartPointer.h"
#include "albaString.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMEGroup.h"

#include <string>
#include <assert.h>


//------------------------------------------------------------------------
void albaOpExporterLandmarkTest::TestOnLandmarkImporter()
{
	int returnValue = -1;

	albaOpImporterLandmark *importer = new albaOpImporterLandmark("importer");
	importer->TestModeOn();

	//Import Data
	albaString filename = ALBA_DATA_ROOT;
	filename << "/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->SetTypeSeparation(3);
	importer->Read();
	albaVMELandmarkCloud *node = (albaVMELandmarkCloud *)importer->GetOutput();
	int numberOfLandmarks = node->GetNumberOfLandmarks();


	//Initialize exporter
	albaOpExporterLandmark *exporter = new albaOpExporterLandmark("test exporter");
	exporter->SetInput(node);
	albaString fileExp = GET_TEST_DATA_DIR();
	fileExp << "/Export2.txt";
	exporter->TestModeOn();
	exporter->SetFileName(fileExp);
	exporter->ExportLandmark();

	//Re-import Exported data
	albaOpImporterLandmark *importer2 = new albaOpImporterLandmark("importer");
	importer2->TestModeOn();
	importer2->SetFileName(fileExp);
	importer2->Read();
	albaVMELandmarkCloud *node2 = (albaVMELandmarkCloud *)importer2->GetOutput();

	int result = ALBA_OK;

	double original[3], exported[3];

	for (int j = 0; j < numberOfLandmarks; j++)
	{
		node->GetLandmarkPosition(j, original);
		node2->GetLandmarkPosition(j, exported);

		if (fabs(original[0] - exported[0]) > 0.01 || fabs(original[1] - exported[1]) > 0.01 || fabs(original[1] - exported[1]) > 0.01)
		{
			result = ALBA_ERROR;
		}
	}

	CPPUNIT_ASSERT(result == ALBA_OK);

	delete exporter;
	delete importer;
	delete importer2;
}

//------------------------------------------------------------------------
void albaOpExporterLandmarkTest::TestMultipleExports()
{
	int returnValue = -1;

	// Import first Landmark
	albaOpImporterLandmark *importer = new albaOpImporterLandmark("importer");
	importer->TestModeOn();

	albaString filename = ALBA_DATA_ROOT;
	filename << "/RAW_MAL/cloud_to_be_imported";
	importer->SetTypeSeparation(3);
	importer->SetFileName(filename.GetCStr());
	importer->Read();
	albaVMELandmarkCloud *node = (albaVMELandmarkCloud *)importer->GetOutput();
	node->SetName("cloud_to_be_imported");

	// Import second Landmark
	albaOpImporterLandmark *importer2 = new albaOpImporterLandmark("importer");
	importer2->TestModeOn();

	albaString filename2 = ALBA_DATA_ROOT;
	filename2 << "/RAW_MAL/cloud_NOT_TAGGED";
	importer2->SetFileName(filename2.GetCStr());
	importer2->SetTypeSeparation(1);
	importer2->SetOnlyCoordinates(true);
	importer2->Read();
	albaVMELandmarkCloud *node2 = (albaVMELandmarkCloud *)importer2->GetOutput();
	node2->SetName("cloud_NOT_TAGGED");

	albaVMELandmarkCloud *node3;
	albaNEW(node3);
	node3->DeepCopy(node);

	// Create group
	albaVMEGroup* group;
	albaNEW(group);
	group->AddChild(node);
	group->AddChild(node2);
	albaVMEGroup* group2;
	albaNEW(group2);
	group->AddChild(group2);
	group2->AddChild(node3);

	// Initialize exporter
	albaOpExporterLandmark *exporter = new albaOpExporterLandmark("test exporter");
	exporter->TestModeOn();

	CPPUNIT_ASSERT(exporter->Accept(group));

	albaString fileExp = GET_TEST_DATA_DIR();
	//fileExp << "/RAW_MAL/";
	exporter->SetDirName(fileExp);
	exporter->OpRun();

	// Test only for second LMC
	fileExp = GET_TEST_DATA_DIR();
	fileExp << "/LC_SET_";
	fileExp << node2->GetName();
	fileExp << ".txt";

	//Re-import Exported data
	albaOpImporterLandmark *importer3 = new albaOpImporterLandmark("importer");
	importer3->TestModeOn();
	importer3->SetFileName(fileExp);
	importer3->Read();
	albaVMELandmarkCloud *node4 = (albaVMELandmarkCloud *)importer3->GetOutput();
	int numberOfLandmarks = node4->GetNumberOfLandmarks();

	int result = ALBA_OK;

	double original[3], exported[3];

	for (int j = 0; j < numberOfLandmarks; j++)
	{
		node2->GetLandmarkPosition(j, original);
		node4->GetLandmarkPosition(j, exported);

		if (fabs(original[0] - exported[0]) > 0.01 || fabs(original[1] - exported[1]) > 0.01 || fabs(original[1] - exported[1]) > 0.01)
		{
			result = ALBA_ERROR;
		}
	}

	CPPUNIT_ASSERT(result == ALBA_OK);

	// Test existence of third LC
	fileExp = GET_TEST_DATA_DIR();
	fileExp << "/LC_SET_";
	fileExp << node3->GetName();
	fileExp << "_renamed_1.txt";
	std::ifstream f_In(fileExp);
	CPPUNIT_ASSERT(f_In);

	delete exporter;
	exporter = NULL;
	node3->ReparentTo(NULL);
	node2->ReparentTo(NULL);
	node->ReparentTo(NULL);
	group2->ReparentTo(NULL);
	albaDEL(node3);
	albaDEL(group2);
	albaDEL(group);
	delete importer;
	delete importer2;
	delete importer3;
}
