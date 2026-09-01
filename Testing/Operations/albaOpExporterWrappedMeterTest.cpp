/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterWrappedMeterTest
 Authors: Daniele Giunchi
 
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

#include "albaOpExporterWrappedMeterTest.h"
#include "albaOpExporterWrappedMeter.h"

#include "albaVMEComputeWrapping.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaOpImporterLandmark.h"
#include "vtkMath.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaString.h"

#include "vtkDataSet.h"

#include <string>
#include <assert.h>
#include <fstream>

enum ENUM_TEST_AXIS
{
	X = 0,
	Y,
	Z,
};

void albaOpExporterWrappedMeterTest::meterImplement(){


	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	//create a sphere vme for wrap

	albaVMESurfaceParametric *vmeSphere;
	albaNEW(vmeSphere);	
	vmeSphere->ReparentTo(storage->GetRoot());
	vmeSphere->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_SPHERE);
	vmeSphere->SetSphereRadius(5.0);
	vmeSphere->Update();

	//create a start vme 
	albaVMESurfaceParametric *vmeSTART;
	albaNEW(vmeSTART);	
	vmeSTART->ReparentTo(storage->GetRoot());
	vmeSTART->Update();


	albaMatrix matrix1;
	matrix1.SetElement(X,3,-10); //set a translation value on x axis of -10.0
	matrix1.SetElement(Y,3,1.5); //set a translation value on y axis of 1.5
	vmeSTART->SetAbsMatrix(matrix1);

	//create a end vme 
	albaVMESurfaceParametric *vmeEND;
	albaNEW(vmeEND);	
	vmeEND->ReparentTo(storage->GetRoot());
	vmeEND->Update();

	albaMatrix matrix2;
	matrix2.SetElement(X,3,10); //set a translation value on x axis of 10.0
	matrix2.SetElement(Y,3,1.5); //set a translation value on y axis of 1.5
	vmeEND->SetAbsMatrix(matrix2);

	//------------create meter--------------------- 
	albaVMEComputeWrapping  *wrappedMeter;
	albaNEW(wrappedMeter);

	wrappedMeter->SetMeterLink("StartVME",vmeSTART);
	wrappedMeter->SetMeterLink("EndVME1",vmeEND);
	wrappedMeter->SetMeterLink("WrappedVME1",vmeSphere);
	wrappedMeter->SetWrappedClass(albaVMEComputeWrapping::NEW_METER);
	wrappedMeter->SetWrappedMode1(albaVMEComputeWrapping::SINGLE_SPHERE);


	wrappedMeter->ReparentTo(storage->GetRoot());

	wrappedMeter->Modified();
	wrappedMeter->Update();

  double testValue = wrappedMeter->GetDistance();
	CPPUNIT_ASSERT(wrappedMeter->GetDistance() > 17.4 && wrappedMeter->GetDistance() < 17.5);

	wrappedMeter->ReparentTo(NULL);
  vmeSphere->ReparentTo(NULL);
  vmeSTART->ReparentTo(NULL);
  vmeEND->ReparentTo(NULL);
  albaDEL(vmeSphere);
  albaDEL(vmeSTART);
  albaDEL(vmeEND);
	albaDEL(wrappedMeter);
	albaDEL(storage);


}
//-----------------------------------------------------------
void albaOpExporterWrappedMeterTest::Test() 
//-----------------------------------------------------------
{
	meterImplement();

}

/*
//create landmarks and relative landmark cloud
albaOpImporterLandmark *importer=new albaOpImporterLandmark("importer");
importer->TestModeOn();
// importer->SetInput(storage->GetRoot());

albaString filename=ALBA_DATA_ROOT;
filename<<"/RAW_MAL/cloud_to_be_imported";


//albaString filename = "C:\\ALBA\\Medical\\Testing\\unittestData\\RAW_MAL\\cloud_to_be_imported";
importer->SetFileName(filename.GetCStr());
importer->Read();
//  importer->ReadWithoutTag();
albaVMELandmarkCloud *cloud=(albaVMELandmarkCloud *)importer->GetOutput();
cloud->Open();



medVMEComputeWrapping *wrappedMeter;
albaNEW(wrappedMeter);

wrappedMeter->SetMeterLink("StartVME",cloud->GetLandmark(0));
wrappedMeter->SetMeterLink("EndVME1",cloud->GetLandmark(1));

wrappedMeter->SetMeterLink(cloud->GetLandmark(2)->GetName(),cloud->GetLandmark(2));
wrappedMeter->SetMeterLink(cloud->GetLandmark(3)->GetName(),cloud->GetLandmark(3));
wrappedMeter->SetMeterLink(cloud->GetLandmark(4)->GetName(),cloud->GetLandmark(4));


wrappedMeter->SetParent(cloud);
wrappedMeter->GetOutput()->GetVTKData()->Update();
wrappedMeter->Modified();
wrappedMeter->Update();

double l1[3], l2[3], l3[3]; 
cloud->GetLandmark(2)->GetPoint(l1);
cloud->GetLandmark(3)->GetPoint(l2);
cloud->GetLandmark(4)->GetPoint(l3);

const short int controlDimension = 9;
double controlValues[controlDimension];
controlValues[0] = l1[0];controlValues[1] = l1[1];controlValues[2] = l1[2];
controlValues[3] = l2[0];controlValues[4] = l2[1];controlValues[5] = l2[2];
controlValues[6] = l3[0];controlValues[7] = l3[1];controlValues[8] = l3[2];


//Inizialize exporter
albaOpExporterWrappedMeter *exporter=new albaOpExporterWrappedMeter("test exporter");
exporter->SetInput(wrappedMeter);
albaString fileExp=ALBA_DATA_ROOT;
fileExp<<"/RAW_MAL/ExportWrappedMeter.txt";
exporter->TestModeOn();
exporter->SetFileName(fileExp);
exporter->ExportWrappedMeterCoordinates(0,0);

std::fstream control(fileExp);

int result = ALBA_OK;
double pos1, pos2, pos3;

short int counter = 0;
while(counter < controlDimension / 3)
{
control >> pos1;
control >> pos2;
control >> pos3;

if((pos1-controlValues[counter*3]  > 0.01)   || 
(pos2-controlValues[counter*3+1] > 0.01)   ||
(pos3-controlValues[counter*3+2] > 0.01)   )
result = ALBA_ERROR;

counter++;
}

CPPUNIT_ASSERT(result == ALBA_OK);
control.close();

wrappedMeter->SetParent(NULL);

albaDEL(exporter);
albaDEL(wrappedMeter);
cppDEL(importer);

//albaDEL(storage);
*/