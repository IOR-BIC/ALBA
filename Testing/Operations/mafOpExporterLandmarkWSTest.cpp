/*=========================================================================

 Program: MAF2
 Module: mafOpExporterLandmarkWSTest
 Authors: Simone Brazzale
 
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
#pragma runtime_checks( "s", off )
#include "mafOpExporterLandmarkWSTest.h"
#include "mafOpExporterLandmarkWS.h"
#include "mafOpImporterLandmarkWS.h"
#include "mafOpImporterLandmark.h"

#include "mafSmartPointer.h"
#include "mafString.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include <string>
#include <assert.h>

//------------------------------------------------------------------------
void mafOpExporterLandmarkWSTest::TestOnLandmarkImporter()
//------------------------------------------------------------------------
{
	int returnValue = -1;

	// Import input file
  mafOpImporterLandmark *importer=new mafOpImporterLandmark("importer");
	importer->TestModeOn();

	mafString filename=MAF_DATA_ROOT;
	filename<<"/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
	mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();

	// Initialize and execute exporter
	mafOpExporterLandmarkWS *exporter=new mafOpExporterLandmarkWS("test exporter");
	exporter->SetInput(node);
	mafString fileExp=MAF_DATA_ROOT;
	fileExp<<"/RAW_MAL/ExportWS.csv";
	exporter->TestModeOn();
	exporter->SetFileName(fileExp);
	exporter->Write();

  // Import output file
  mafOpImporterLandmarkWS *importerWS=new mafOpImporterLandmarkWS("importer");
	importerWS->TestModeOn();

	filename=MAF_DATA_ROOT;
	filename<<"/RAW_MAL/ExportWS.csv";
	importerWS->SetFileName(filename.GetCStr());
	importerWS->Read();
	mafVMELandmarkCloud *node_WS=(mafVMELandmarkCloud *)importerWS->GetOutput();



	int result = MAF_OK;
	std::vector<double *> coord;
	std::vector<double *> coord_WS;
	
  int numberOfLandmarks = ((mafVMELandmarkCloud *)node)->GetNumberOfLandmarks();
	for(int j=0 ; j < numberOfLandmarks; j++)
	{
		mafVMELandmark *landmark = ((mafVMELandmark *)((mafVMELandmarkCloud *)node)->GetLandmark(j));
		double *xyz = new double[3];
		double rot[3];
		landmark->GetOutput()->GetPose(xyz , rot , 0);
		coord.push_back(xyz);
		coord[coord.size()-1][0] = xyz[0];
		coord[coord.size()-1][1] = xyz[1];
		coord[coord.size()-1][2] = xyz[2];
	}

  numberOfLandmarks = ((mafVMELandmarkCloud *)node_WS)->GetNumberOfLandmarks();
  for(int j=0 ; j < numberOfLandmarks; j++)
	{
		mafVMELandmark *landmark = ((mafVMELandmark *)((mafVMELandmarkCloud *)node_WS)->GetLandmark(j));
		double *xyz = new double[3];
		double rot[3];
		landmark->GetOutput()->GetPose(xyz , rot , 0);
		coord_WS.push_back(xyz);
		coord_WS[coord_WS.size()-1][0] = xyz[0];
		coord_WS[coord_WS.size()-1][1] = xyz[1];
		coord_WS[coord_WS.size()-1][2] = xyz[2];
	}

  CPPUNIT_ASSERT(coord.size()==coord_WS.size());
  
  for (int i=0; i<coord.size();i++)
  {
    if (abs(coord[i][0]-coord_WS[i][0])>0.05)
      result = 1;
    if (abs(coord[i][1]-coord_WS[i][1])>0.05)
      result = 1;
    if (abs(coord[i][2]-coord_WS[i][2])>0.05)
      result = 1;
  }
	
	CPPUNIT_ASSERT(result == MAF_OK);

	for(int i=0; i< coord.size(); i++)
	{
		delete coord[i];
	}
  for(int i=0; i< coord_WS.size(); i++)
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