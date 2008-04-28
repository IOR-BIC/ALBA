/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpExporterWrappedMeterTest.cpp,v $
Language:  C++
Date:      $Date: 2008-04-28 08:58:51 $
Version:   $Revision: 1.2 $
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

#include "medVMEWrappedMeter.h"
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

//-----------------------------------------------------------
void medOpExporterWrappedMeterTest::Test() 
//-----------------------------------------------------------
{
  /*mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();*/


  //create landmarks and relative landmark cloud
  medOpImporterLandmark *importer=new medOpImporterLandmark("importer");
  importer->TestModeOn();
  //importer->SetInput(storage->GetRoot());
  mafString filename=MED_DATA_ROOT;
  filename<<"/RAW_MAL/cloud_to_be_imported";
  importer->SetFileName(filename.GetCStr());
  importer->Read();
  mafVMELandmarkCloud *cloud=(mafVMELandmarkCloud *)importer->GetOutput();
  cloud->Open();

  //cloud->SetParent(storage->GetRoot());

  medVMEWrappedMeter *wrappedMeter;
  mafNEW(wrappedMeter);

  wrappedMeter->SetMeterLink("StartVME",cloud->GetLandmark(0));
  wrappedMeter->SetMeterLink("EndVME1",cloud->GetLandmark(1));

  wrappedMeter->SetMeterLink(cloud->GetLandmark(2)->GetName(),cloud->GetLandmark(2));
  wrappedMeter->SetMeterLink(cloud->GetLandmark(3)->GetName(),cloud->GetLandmark(3));
  wrappedMeter->SetMeterLink(cloud->GetLandmark(4)->GetName(),cloud->GetLandmark(4));
  //wrappedMeter->PushIdVector(cloud->GetId()); // for landmark middlepoint is memorized as sequence of cloud id and interal id of the landmark
  //wrappedMeter->PushIdVector(2); //this is for the vector syncronized with the gui widget, that is not used in gui test


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

  /*printf("\n%.2f %.2f %.2f\n", l1[0] , l1[1], l1[2]);
  printf("\n%.2f %.2f %.2f\n", l2[0] , l2[1], l2[2]);
  printf("\n%.2f %.2f %.2f\n", l3[0] , l3[1], l3[2]);*/

  //Inizialize exporter
  medOpExporterWrappedMeter *exporter=new medOpExporterWrappedMeter("test exporter");
  exporter->SetInput(wrappedMeter);
  mafString fileExp=MED_DATA_ROOT;
  fileExp<<"/RAW_MAL/ExportWrappedMeter.txt";
  exporter->TestModeOn();
  exporter->SetFileName(fileExp);
  exporter->ExportWrappedMeterCoordinates();

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

}
