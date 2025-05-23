/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMetersTest
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
//#pragma runtime_checks( "s", off )

#include <cppunit/config/SourcePrefix.h>
#include "albaOpExporterMetersTest.h"
#include "albaOpExporterMeters.h"
#include "albaOpImporterLandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMEWrappedMeter.h"
#include "albaVMEMeter.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEOutputSurface.h"

#include "vtkDataSet.h"
#include "vtkMath.h"


#include <string>
#include <assert.h>

#define TEST_RESULT CPPUNIT_ASSERT(result)

//----------------------------------------------------------------------------
void albaOpExporterMetersTest::TestFixture()
//----------------------------------------------------------------------------
{
  albaOpExporterMetersTest *op = NULL;
  op = new albaOpExporterMetersTest();
  delete op;
}

//------------------------------------------------------------------------
void albaOpExporterMetersTest::TestOnMetersImporter()
//------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
	
  //create landmarks and relative landmark cloud
  albaOpImporterLandmark *importer=new albaOpImporterLandmark("importer");
  importer->TestModeOn();
  //importer->SetInput(storage->GetRoot());
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/RAW_MAL/cloud_to_be_imported";
  importer->SetFileName(filename.GetCStr());
  importer->Read();
  albaVMELandmarkCloud *cloud=(albaVMELandmarkCloud *)importer->GetOutput();

  storage->GetRoot()->AddChild(cloud);
  //cloud->SetParent(storage->GetRoot());

  albaVMEWrappedMeter *wrappedMeterManual;
  albaNEW(wrappedMeterManual);
  wrappedMeterManual->SetMeterLink("StartVME",cloud->GetLandmark(0));
  wrappedMeterManual->SetMeterLink("EndVME1",cloud->GetLandmark(1));
  wrappedMeterManual->SetMeterLink(cloud->GetLandmark(2)->GetName(),cloud->GetLandmark(2));
  wrappedMeterManual->SetMeterLink(cloud->GetLandmark(3)->GetName(),cloud->GetLandmark(3));
  wrappedMeterManual->SetMeterLink(cloud->GetLandmark(4)->GetName(),cloud->GetLandmark(4));
  storage->GetRoot()->AddChild(wrappedMeterManual);
  wrappedMeterManual->GetOutput()->GetVTKData()->Update();
  wrappedMeterManual->SetName("TestManualWrappedMeter");
  wrappedMeterManual->Modified();
  wrappedMeterManual->Update();

  //parametric surface for wrapped automatic meter
  albaVMESurfaceParametric *vmeParametricSurfaceWrapped;
  albaNEW(vmeParametricSurfaceWrapped);
  storage->GetRoot()->AddChild(vmeParametricSurfaceWrapped);
  vmeParametricSurfaceWrapped->SetSphereRadius(2.);
  vmeParametricSurfaceWrapped->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceWrapped->Update();

  double t0[3], t1[3];
  cloud->GetLandmark(0)->GetPoint(t0);
  cloud->GetLandmark(1)->GetPoint(t1);
  double translation[3];
  translation[0] = (t0[0] + t1[0])/2.;
  translation[1] = (t0[1] + t1[1])/2.;
  translation[2] = (t0[2] + t1[2])/2.;
  
  vmeParametricSurfaceWrapped->GetSurfaceOutput()->GetMatrix()->SetElement(0,3, translation[0]);
  vmeParametricSurfaceWrapped->GetSurfaceOutput()->GetMatrix()->SetElement(1,3, translation[1]);
  vmeParametricSurfaceWrapped->GetSurfaceOutput()->GetMatrix()->SetElement(2,3, translation[2]);
  vmeParametricSurfaceWrapped->Modified();
  vmeParametricSurfaceWrapped->Update();

  //automatic meter
  albaVMEWrappedMeter *wrappedMeterAutomatic;
  albaNEW(wrappedMeterAutomatic);
  wrappedMeterAutomatic->SetMeterLink("StartVME",cloud->GetLandmark(0));
  wrappedMeterAutomatic->SetMeterLink("EndVME1",cloud->GetLandmark(1));
  wrappedMeterAutomatic->SetMeterLink("WrappedVME",vmeParametricSurfaceWrapped);
  wrappedMeterAutomatic->SetWrappedMode(albaVMEWrappedMeter::AUTOMATED_WRAP);
  storage->GetRoot()->AddChild(wrappedMeterAutomatic);
  wrappedMeterAutomatic->GetOutput()->GetVTKData()->Update();
  wrappedMeterAutomatic->SetName("TestAutomaticWrappedMeter");
  wrappedMeterAutomatic->Modified();
  wrappedMeterAutomatic->Update();

  //meter
  albaVMEMeter *meter;
  albaNEW(meter);
  meter->SetMeterLink("StartVME",cloud->GetLandmark(0));
  meter->SetMeterLink("EndVME1",cloud->GetLandmark(1));
  storage->GetRoot()->AddChild(meter);
  meter->GetOutput()->GetVTKData()->Update();
  meter->SetName("TestClassicMeter");
  meter->Modified();
  meter->Update();

  //wrapped meter
  double l1[3], l2[3], l3[3]; 
  cloud->GetLandmark(2)->GetPoint(l1);
  cloud->GetLandmark(3)->GetPoint(l2);
  cloud->GetLandmark(4)->GetPoint(l3);

  const short int controlDimensionManual = 15;
  double controlValuesManual[controlDimensionManual];
  controlValuesManual[0] = t0[0];controlValuesManual[1] = t0[1];controlValuesManual[2] = t0[2];
  controlValuesManual[3] = l1[0];controlValuesManual[4] = l1[1];controlValuesManual[5] = l1[2];
  controlValuesManual[6] = l2[0];controlValuesManual[7] = l2[1];controlValuesManual[8] = l2[2];
  controlValuesManual[9] = l3[0];controlValuesManual[10] = l3[1];controlValuesManual[11] = l3[2];
  controlValuesManual[12] = t1[0];controlValuesManual[13] = t1[1];controlValuesManual[14] = t1[2];

  /*printf("\n%.2f %.2f %.2f\n", l1[0] , l1[1], l1[2]);
  printf("\n%.2f %.2f %.2f\n", l2[0] , l2[1], l2[2]);
  printf("\n%.2f %.2f %.2f\n", l3[0] , l3[1], l3[2]);*/

  //Inizialize exporter
  albaOpExporterMeters *exporter=new albaOpExporterMeters("test exporter");
  exporter->SetInput(wrappedMeterManual);
	albaString fileExp = GET_TEST_DATA_DIR();
  fileExp<<"//ExportMeters.txt";
  exporter->TestModeOn();
  exporter->SetFileName(fileExp);
  exporter->Export();

  std::fstream control(fileExp);

  int result = ALBA_OK;
  double pos1, pos2, pos3;

  short int counter = 0;

  double time;
  control >> time;
  result = time == 0.;
  TEST_RESULT;

  //TestManualWrappedMeter
  std::string name;
  control >> name;

  result = name == "TestManualWrappedMeter";
  TEST_RESULT;

  int res = ALBA_OK;
  while(counter < controlDimensionManual / 3)
  {
    control >> name; //points name
    control >> pos1;
    control >> pos2;
    control >> pos3;

    if((pos1-controlValuesManual[counter*3]  > 0.01)   || 
      (pos2-controlValuesManual[counter*3+1] > 0.01)   ||
      (pos3-controlValuesManual[counter*3+2] > 0.01)   )
      res = ALBA_ERROR;

    counter++;
  }

  //test wrapped meter automatic
  counter = 0;
  const short int controlDimensionAutomated = 12;
  double controlValuesAutomated[controlDimensionAutomated];
  controlValuesAutomated[0] = t0[0];controlValuesAutomated[1] = t0[1];controlValuesAutomated[2] = t0[2];
  controlValuesAutomated[3] = wrappedMeterAutomatic->GetWrappedGeometryTangent1()[0];controlValuesAutomated[4] = wrappedMeterAutomatic->GetWrappedGeometryTangent1()[1];controlValuesAutomated[5] = wrappedMeterAutomatic->GetWrappedGeometryTangent1()[2];
  controlValuesAutomated[6] = wrappedMeterAutomatic->GetWrappedGeometryTangent2()[0];controlValuesAutomated[7] = wrappedMeterAutomatic->GetWrappedGeometryTangent2()[1];controlValuesAutomated[8] = wrappedMeterAutomatic->GetWrappedGeometryTangent2()[2];
  controlValuesAutomated[9] = t1[0];controlValuesAutomated[10] = t1[1];controlValuesAutomated[11] = t1[2];

  control >> name;
  result = name == "TestAutomaticWrappedMeter";
  TEST_RESULT;

  while(counter < controlDimensionAutomated / 3)
  {
    control >> name; //points name
    control >> pos1;
    control >> pos2;
    control >> pos3;

    if((pos1-controlValuesAutomated[counter*3]  > 0.01)   || 
      (pos2-controlValuesAutomated[counter*3+1] > 0.01)   ||
      (pos3-controlValuesAutomated[counter*3+2] > 0.01)   )
      res = ALBA_ERROR;

    counter++;
  }

  //test classic meter
  counter = 0;
  const short int controlDimensionAClassic = 6;
  double controlValuesClassic[controlDimensionAClassic];
  controlValuesClassic[0] = t0[0];controlValuesClassic[1] = t0[1];controlValuesClassic[2] = t0[2];
  controlValuesClassic[3] = t1[0];controlValuesClassic[4] = t1[1];controlValuesClassic[5] = t1[2];

  control >> name;
  result = name == "TestClassicMeter";
  TEST_RESULT;

  while(counter < controlDimensionAClassic / 3)
  {
    control >> name; //points name
    control >> pos1;
    control >> pos2;
    control >> pos3;

    if((pos1-controlValuesClassic[counter*3]  > 0.01)   || 
      (pos2-controlValuesClassic[counter*3+1] > 0.01)   ||
      (pos3-controlValuesClassic[counter*3+2] > 0.01)   )
      res = ALBA_ERROR;

    counter++;
  }

  control.close();

  result = res == ALBA_OK;
  TEST_RESULT;

  wrappedMeterManual->ReparentTo(NULL);
  vmeParametricSurfaceWrapped->ReparentTo(NULL);
  meter->ReparentTo(NULL);
  wrappedMeterAutomatic->ReparentTo(NULL);

  albaDEL(exporter);
  albaDEL(wrappedMeterManual);
  albaDEL(meter);
  albaDEL(wrappedMeterAutomatic);
  albaDEL(vmeParametricSurfaceWrapped);
  
  cppDEL(importer);

  albaDEL(storage);
}
