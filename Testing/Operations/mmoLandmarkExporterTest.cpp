/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmoLandmarkExporterTest.cpp,v $
Language:  C++
Date:      $Date: 2007-09-16 22:24:57 $
Version:   $Revision: 1.4 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#pragma runtime_checks( "s", off )
#include "mmoLandmarkExporterTest.h"
#include "mmoLandmarkExporter.h"
#include "mmoLandmarkImporter.h"

#include "mafVMERawMotionData.h"
#include "mafSmartPointer.h"
#include "mafString.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void mmoLandmarkExporterTest::TestOnVmeRawMotionData() 
//-----------------------------------------------------------
{
  int returnValue = -1;

  // reader
  mafVMERawMotionData *vmeRawMotionData;
	mafNEW(vmeRawMotionData);

  // try to set this data to the volume
  mafString filename=MED_DATA_ROOT;
  filename<<"/RAW_MAL/Fprg3bsi_NOT_USED.man";

  mafString fileDict=MED_DATA_ROOT;
  fileDict<<"/RAW_MAL/DICT_FPRG3BSL_421_NOT_USED.TXT";

  vmeRawMotionData->DictionaryOn();
  vmeRawMotionData->SetFileName(filename);
	vmeRawMotionData->SetDictionaryFileName(fileDict);

  // read control
  vmeRawMotionData->Read();


	//Inizialize exporter
	mmoLandmarkExporter *exporter=new mmoLandmarkExporter("test exporter");
	exporter->SetInput(vmeRawMotionData);
	mafString fileExp=MED_DATA_ROOT;
  fileExp<<"/RAW_MAL/Export.txt";
  exporter->TestModeOn();
	exporter->SetFileName(fileExp);
  exporter->ExportLandmark();


  for(int i =0 ; i< 5 ; i++)
  {
    ((mafVMELandmarkCloud *)vmeRawMotionData->GetChild(i))->Open();
  }
  
  int result = MAF_OK;
  std::vector<double *> coord;
  for(int k=0; k<3; k++)
  {
    for(int i =0 ; i< 5 ; i++)
    {
      for(int j=0 ; j < ((mafVMELandmarkCloud *)vmeRawMotionData->GetChild(i))->GetNumberOfLandmarks(); j++)
      {
        mafVMELandmark *landmark = ((mafVMELandmark *)((mafVMELandmarkCloud *)vmeRawMotionData->GetChild(i))->GetLandmark(j));
        double *xyz = new double[3];
        double rot[3];
        landmark->GetOutput()->GetPose(xyz , rot , k);
        coord.push_back(xyz);
      }
    }
  }


  char text[10] = "";
  char name[10];

  double *pos1 = new double;
  double *pos2 = new double;
  double *pos3 = new double;


  std::fstream control(fileExp);
  
  for(int i =0 ; i< 2; i++)
  {
    //first line
    control.getline(text, 10);
    if(i != 0)
      control.getline(text, 10);

    for(int j=0 ; j < 22; j++)  // limited to the the first three landmarks
    {
      
      control >> name;
      control >> *pos1;
      control >> *pos2;
      control >> *pos3;

     double dx = coord[i*22+j][0];
     double dy = coord[i*22+j][1];
     double dz = coord[i*22+j][2];
     if(fabs(dx - *pos1)<0.01 && fabs(dy - *pos2)<0.01 && fabs(dz - *pos3)<0.01);
     else
     {
       result = -1;
     }
     
     
    }

  }
  delete pos1;
	delete pos2;
	delete pos3;
  CPPUNIT_ASSERT(result == MAF_OK);

	for(int i=0; i<coord.size();i++)
	{
		delete coord[i];
	}

  control.close();
  coord.clear();

	mafDEL(vmeRawMotionData);

  delete exporter;
  exporter = NULL;
	
  
}
//------------------------------------------------------------------------
void mmoLandmarkExporterTest::TestOnLandmarkImporter()
//------------------------------------------------------------------------
{
	int returnValue = -1;

	mmoLandmarkImporter *importer=new mmoLandmarkImporter("importer");
	importer->TestModeOn();

	mafString filename=MED_DATA_ROOT;
	filename<<"/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
	mafVMELandmarkCloud *node=(mafVMELandmarkCloud *)importer->GetOutput();


	//Inizialize exporter
	mmoLandmarkExporter *exporter=new mmoLandmarkExporter("test exporter");
	exporter->SetInput(node);
	mafString fileExp=MED_DATA_ROOT;
	fileExp<<"/RAW_MAL/Export2.txt";
	exporter->TestModeOn();
	exporter->SetFileName(fileExp);
	exporter->ExportLandmark();



	((mafVMELandmarkCloud *)node)->Open();


	int result = MAF_OK;
	std::vector<double *> coord;
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

	char text[10] = "";
	char name[10];

	double pos1;
	double pos2;
	double pos3;


	std::fstream control(fileExp);

	for(int i =0 ; i< 1; i++)
	{
		//first line
		control.getline(text, 10);
		if(i != 0)
			control.getline(text, 10);

		for(int j=0 ; j < numberOfLandmarks; j++)  // limited to the the first three landmarks
		{

			control >> name;
			control >> pos1;
			control >> pos2;
			control >> pos3;

			double dx = coord[i*numberOfLandmarks+j][0];
			double dy = coord[i*numberOfLandmarks+j][1];
			double dz = coord[i*numberOfLandmarks+j][2];
			if(fabs(dx - pos1)<0.01 && fabs(dy - pos2)<0.01 && fabs(dz - pos3)<0.01);
			else
			{
				result = -1;
			} 

		}

	}

	CPPUNIT_ASSERT(result == MAF_OK);


	for(int i=0; i< coord.size(); i++)
	{
		delete coord[i];
	}

	control.close();
	coord.clear();


	delete exporter;
	delete importer;
	exporter = NULL;
	importer = NULL;
}