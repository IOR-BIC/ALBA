/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDataPipeCustomProberTest.cpp,v $
Language:  C++
Date:      $Date: 2009-06-17 13:54:09 $
Version:   $Revision: 1.1.2.1 $
Authors:   Robero Mucci
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafDataPipeCustomProberTest.h"

#include "mafDataPipeCustomProber.h"
#include "mafDataPipe.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMEImage.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafOpImporterVTK.h"

#include "vtkDataSet.h"
#include "vtkFloatArray.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"

#define TEST_RESULT CPPUNIT_ASSERT(result);


//----------------------------------------------------------------------------
void mafDataPipeCustomProberTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeCustomProberTest::setUp()
//----------------------------------------------------------------------------
{
  result = false;
}
//----------------------------------------------------------------------------
void mafDataPipeCustomProberTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDataPipeCustomProberTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafDataPipeCustomProber> di;

	mafDataPipeCustomProber * di2;
	mafNEW(di2);
	mafDEL(di2);
}

//----------------------------------------------------------------------------
void mafDataPipeCustomProberTest::TestExecute()
//----------------------------------------------------------------------------
{
  //Create storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  //import volume
  mafOpImporterVTK *importer=new mafOpImporterVTK("importer");
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeCustomProber/volume.vtk";
  importer->TestModeOn();
  importer->SetFileName(filename);
  importer->SetInput(storage->GetRoot());
  importer->ImportVTK();
  mafSmartPointer<mafVMEVolumeGray> volume = mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
  volume->Update();
  double rangeVol[2];
  volume->GetOutput()->Update();
  volume->GetOutput()->GetVTKData()->GetScalarRange(rangeVol);

  //import surface
  mafOpImporterVTK *importer1=new mafOpImporterVTK("importer");
  filename=MAF_DATA_ROOT;
  filename<<"/Test_PipeCustomProber/surface.vtk";
  importer1->TestModeOn();
  importer1->SetFileName(filename);
  importer1->SetInput(storage->GetRoot());
  importer1->ImportVTK();
  mafSmartPointer<mafVMESurface> surface = mafVMESurface::SafeDownCast(importer1->GetOutput());
  surface->Update();
  
  //create dataset
  mafSmartPointer<mafVMEImage> surfaceVME;
  
  mafSmartPointer<mafDataPipeCustomProber>  probingDataPipe;
  probingDataPipe->SetDependOnAbsPose(true);
  probingDataPipe->SetMode(mafDataPipeCustomProber::DISTANCE_MODE_SCALAR);

  probingDataPipe->SetVolume(volume);
  probingDataPipe->SetSurface(surface);
  probingDataPipe->SetVME(surfaceVME);
  probingDataPipe->OnEvent(&mafEventBase(probingDataPipe->GetVTKDataPipe(),VME_OUTPUT_DATA_PREUPDATE));

  double range[2];
  probingDataPipe->GetVTKData()->Update();
  probingDataPipe->GetVTKData()->GetScalarRange(range);

  result = (rangeVol[0] <= range[0] && rangeVol[1] >= range[1]);
  TEST_RESULT;

  mafDEL(storage);
  mafDEL(importer);
  mafDEL(importer1);
  

}
//----------------------------------------------------------------------------
void mafDataPipeCustomProberTest::TestSetSurface()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> surfaceIN;
  mafNode *node;
  mafSmartPointer<mafDataPipeCustomProber>  probingDataPipe;
  probingDataPipe->SetSurface(surfaceIN);

  node = probingDataPipe->GetSurface();
  mafSmartPointer<mafVMESurface> surfaceOUT = mafVMESurface::SafeDownCast(node);

  result = surfaceIN->Equals(surfaceOUT);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void mafDataPipeCustomProberTest::TestSetVolume()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volumeIN;
  mafNode *node;
  mafSmartPointer<mafDataPipeCustomProber>  probingDataPipe;
  probingDataPipe->SetSurface(volumeIN);

  node = probingDataPipe->GetSurface();
  mafSmartPointer<mafVMEVolumeGray> volumeOUT = mafVMEVolumeGray::SafeDownCast(node);

  result = volumeIN->Equals(volumeOUT);
  TEST_RESULT;

}
//----------------------------------------------------------------------------
void mafDataPipeCustomProberTest::TestSetGet()
//----------------------------------------------------------------------------
{
  //SetMode
  int modeIN = mafDataPipeCustomProber::DISTANCE_MODE;
  mafSmartPointer<mafDataPipeCustomProber>  probingDataPipe;
  probingDataPipe->SetMode(modeIN);
  int modeOUT = probingDataPipe->GetMode();

  result = modeIN == modeOUT;
  TEST_RESULT;


  //SetDistanceThreshold
  float thrIN = 0.2;
  probingDataPipe->SetDistanceThreshold(thrIN);
  float thrOUT = probingDataPipe->GetDistanceThreshold();

  result = mafEquals(thrIN, thrOUT);
  TEST_RESULT;


  //SetMaxDistance
  float distanceIN = 15.1;
  probingDataPipe->SetMaxDistance(distanceIN);
  float distanceOUT = probingDataPipe->GetMaxDistance();

  result = mafEquals(distanceIN, distanceOUT);
  TEST_RESULT;


  //SetHighDensity
  float densityIN = 12.6;
  probingDataPipe->SetHighDensity(densityIN);
  float densityOUT = probingDataPipe->GetHighDensity();

  result = mafEquals(densityIN, densityOUT);
  TEST_RESULT;


  //SetLowDensity
  float lowDensityIN = 25.3;
  probingDataPipe->SetLowDensity(lowDensityIN);
  float lowDensityOUT = probingDataPipe->GetLowDensity();

  result = mafEquals(lowDensityIN, lowDensityOUT);
  TEST_RESULT;

}