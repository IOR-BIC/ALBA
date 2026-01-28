/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeCustomProberTest
 Authors: Robero Mucci
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaDataPipeCustomProberTest.h"

#include "albaDataPipeCustomProber.h"
#include "albaDataPipe.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMEImage.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaOpImporterVTK.h"

#include "vtkDataSet.h"
#include "vtkFloatArray.h"
#include "vtkALBASmartPointer.h"
#include "albaSmartPointer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void albaDataPipeCustomProberTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaDataPipeCustomProberTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaDataPipeCustomProber> di;

	albaDataPipeCustomProber * di2;
	albaNEW(di2);
	albaDEL(di2);
}

//----------------------------------------------------------------------------
void albaDataPipeCustomProberTest::TestExecute()
//----------------------------------------------------------------------------
{
  //Create storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  //import volume
  albaOpImporterVTK *importer=new albaOpImporterVTK("importer");
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipeCustomProber/volume.vtk";
  importer->TestModeOn();
  importer->SetFileName(filename);
  importer->SetInput(storage->GetRoot());
  importer->ImportFile();
  albaSmartPointer<albaVMEVolumeGray> volume = albaVMEVolumeGray::SafeDownCast(importer->GetOutput());
  volume->Update();
  double rangeVol[2];
  volume->GetOutput()->Update();
  volume->GetOutput()->GetVTKData()->GetScalarRange(rangeVol);

  //import surface
  albaOpImporterVTK *importer1=new albaOpImporterVTK("importer");
  filename=ALBA_DATA_ROOT;
  filename<<"/Test_PipeCustomProber/surface.vtk";
  importer1->TestModeOn();
  importer1->SetFileName(filename);
  importer1->SetInput(storage->GetRoot());
  importer1->ImportFile();
  albaSmartPointer<albaVMESurface> surface = albaVMESurface::SafeDownCast(importer1->GetOutput());
  surface->Update();
  
  //create dataset
  albaSmartPointer<albaVMEImage> surfaceVME;
  
  albaSmartPointer<albaDataPipeCustomProber>  probingDataPipe;
  probingDataPipe->SetDependOnAbsPose(true);
  probingDataPipe->SetMode(albaDataPipeCustomProber::DISTANCE_MODE_SCALAR);

  probingDataPipe->SetVolume(volume);
  probingDataPipe->SetSurface(surface);
  probingDataPipe->SetVME(surfaceVME);
  probingDataPipe->OnEvent(&albaEventBase(probingDataPipe->GetVTKDataPipe(),VME_OUTPUT_DATA_PREUPDATE));

  double range[2];
  probingDataPipe->GetVTKData()->Update();
  probingDataPipe->GetVTKData()->GetScalarRange(range);

  result = (rangeVol[0] <= range[0] && rangeVol[1] >= range[1]);
  TEST_RESULT;

  albaDEL(storage);
  albaDEL(importer);
  albaDEL(importer1);
  

}
//----------------------------------------------------------------------------
void albaDataPipeCustomProberTest::TestSetSurface()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> surfaceIN;
  albaVME *node;
  albaSmartPointer<albaDataPipeCustomProber>  probingDataPipe;
  probingDataPipe->SetSurface(surfaceIN);

  node = probingDataPipe->GetSurface();
  albaSmartPointer<albaVMESurface> surfaceOUT = albaVMESurface::SafeDownCast(node);

  result = surfaceIN->Equals(surfaceOUT);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void albaDataPipeCustomProberTest::TestSetVolume()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volumeIN;
  albaVME *node;
  albaSmartPointer<albaDataPipeCustomProber>  probingDataPipe;
  probingDataPipe->SetSurface(volumeIN);

  node = probingDataPipe->GetSurface();
  albaSmartPointer<albaVMEVolumeGray> volumeOUT = albaVMEVolumeGray::SafeDownCast(node);

  result = volumeIN->Equals(volumeOUT);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void albaDataPipeCustomProberTest::TestSetGet()
//----------------------------------------------------------------------------
{
  //SetMode
  int modeIN = albaDataPipeCustomProber::DISTANCE_MODE;
  albaSmartPointer<albaDataPipeCustomProber>  probingDataPipe;
  probingDataPipe->SetMode(modeIN);
  int modeOUT = probingDataPipe->GetMode();

  result = modeIN == modeOUT;
  TEST_RESULT;


  //SetDistanceThreshold
  float thrIN = 0.2;
  probingDataPipe->SetDistanceThreshold(thrIN);
  float thrOUT = probingDataPipe->GetDistanceThreshold();

  result = albaEquals(thrIN, thrOUT);
  TEST_RESULT;


  //SetMaxDistance
  float distanceIN = 15.1;
  probingDataPipe->SetMaxDistance(distanceIN);
  float distanceOUT = probingDataPipe->GetMaxDistance();

  result = albaEquals(distanceIN, distanceOUT);
  TEST_RESULT;


  //SetHighDensity
  float densityIN = 12.6;
  probingDataPipe->SetHighDensity(densityIN);
  float densityOUT = probingDataPipe->GetHighDensity();

  result = albaEquals(densityIN, densityOUT);
  TEST_RESULT;


  //SetLowDensity
  float lowDensityIN = 25.3;
  probingDataPipe->SetLowDensity(lowDensityIN);
  float lowDensityOUT = probingDataPipe->GetLowDensity();

  result = albaEquals(lowDensityIN, lowDensityOUT);
  TEST_RESULT;

}