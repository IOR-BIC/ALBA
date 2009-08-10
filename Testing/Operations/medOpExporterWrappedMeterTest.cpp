/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpExporterWrappedMeterTest.cpp,v $
Language:  C++
Date:      $Date: 2009-08-10 12:59:34 $
Version:   $Revision: 1.2.2.4 $
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

#include "medOpExporterWrappedMeterTest.h"
#include "medOpExporterWrappedMeter.h"

#include "medVMEComputeWrapping.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "medOpImporterLandmark.h"
#include "vtkMath.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafString.h"

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

void medOpExporterWrappedMeterTest::meterImplement(){


	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	//create a sphere vme for wrap

	mafVMESurfaceParametric *vmeSphere;
	mafNEW(vmeSphere);	
	vmeSphere->GetOutput()->GetVTKData()->Update();
	vmeSphere->SetParent(storage->GetRoot());
	vmeSphere->SetGeometryType(mafVMESurfaceParametric::PARAMETRIC_SPHERE);
	vmeSphere->SetSphereRadius(5.0);
	vmeSphere->Update();

	//create a start vme 
	mafVMESurfaceParametric *vmeSTART;
	mafNEW(vmeSTART);	
	vmeSTART->GetOutput()->GetVTKData()->Update();
	vmeSTART->SetParent(storage->GetRoot());
	vmeSTART->Update();


	mafMatrix *matrix1 = vmeSTART->GetOutput()->GetAbsMatrix();
	matrix1->SetElement(X,3,-10); //set a translation value on x axis of -10.0
	matrix1->SetElement(Y,3,1.5); //set a translation value on y axis of 1.5

	//create a end vme 
	mafVMESurfaceParametric *vmeEND;
	mafNEW(vmeEND);	
	vmeEND->GetOutput()->GetVTKData()->Update();
	vmeEND->SetParent(storage->GetRoot());
	vmeEND->Update();

	mafMatrix *matrix2 = vmeEND->GetOutput()->GetAbsMatrix();
	matrix2->SetElement(X,3,10); //set a translation value on x axis of 10.0
	matrix2->SetElement(Y,3,1.5); //set a translation value on y axis of 1.5

	//------------create meter--------------------- 
	medVMEComputeWrapping  *wrappedMeter;
	mafNEW(wrappedMeter);

	wrappedMeter->SetMeterLink("StartVME",vmeSTART);
	wrappedMeter->SetMeterLink("EndVME1",vmeEND);
	wrappedMeter->SetMeterLink("WrappedVME1",vmeSphere);
	wrappedMeter->SetWrappedClass(medVMEComputeWrapping::NEW_METER);
	wrappedMeter->SetWrappedMode1(medVMEComputeWrapping::SINGLE_SPHERE);


	wrappedMeter->SetParent(storage->GetRoot());

	wrappedMeter->GetOutput()->GetVTKData()->Update();
	wrappedMeter->Modified();
	wrappedMeter->Update();

  double testValue = wrappedMeter->GetDistance();
	CPPUNIT_ASSERT(wrappedMeter->GetDistance() > 17.4 && wrappedMeter->GetDistance() < 17.5);

	wrappedMeter->SetParent(NULL);
  vmeSphere->SetParent(NULL);
  vmeSTART->SetParent(NULL);
  vmeEND->SetParent(NULL);
  mafDEL(vmeSphere);
  mafDEL(vmeSTART);
  mafDEL(vmeEND);
	mafDEL(wrappedMeter);
	mafDEL(storage);


}
//-----------------------------------------------------------
void medOpExporterWrappedMeterTest::Test() 
//-----------------------------------------------------------
{
	meterImplement();

}

/*
//create landmarks and relative landmark cloud
medOpImporterLandmark *importer=new medOpImporterLandmark("importer");
importer->TestModeOn();
// importer->SetInput(storage->GetRoot());

mafString filename=MED_DATA_ROOT;
filename<<"/RAW_MAL/cloud_to_be_imported";


//mafString filename = "C:\\MAF\\Medical\\Testing\\unittestData\\RAW_MAL\\cloud_to_be_imported";
importer->SetFileName(filename.GetCStr());
importer->Read();
//  importer->ReadWithoutTag();
mafVMELandmarkCloud *cloud=(mafVMELandmarkCloud *)importer->GetOutput();
cloud->Open();



medVMEComputeWrapping *wrappedMeter;
mafNEW(wrappedMeter);

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
medOpExporterWrappedMeter *exporter=new medOpExporterWrappedMeter("test exporter");
exporter->SetInput(wrappedMeter);
mafString fileExp=MED_DATA_ROOT;
fileExp<<"/RAW_MAL/ExportWrappedMeter.txt";
exporter->TestModeOn();
exporter->SetFileName(fileExp);
exporter->ExportWrappedMeterCoordinates(0,0);

std::fstream control(fileExp);

int result = MAF_OK;
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
result = MAF_ERROR;

counter++;
}

CPPUNIT_ASSERT(result == MAF_OK);
control.close();

wrappedMeter->SetParent(NULL);

mafDEL(exporter);
mafDEL(wrappedMeter);
cppDEL(importer);

//mafDEL(storage);
*/