/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEProberTest
 Authors: Eleonora Mambrini
 
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

#include "albaVMEProberTest.h"
#include "albaVMEProber.h"

#include <wx/dir.h>

#include "albaVMEImage.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaDataPipeCustomProber.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEOutput.h"

#include "vtkDataSet.h"


//----------------------------------------------------------------------------
void albaVMEProberTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{

  albaVMEProber *prober;
  albaNEW(prober);
  albaDEL(prober);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetVisualPipeTest()
//----------------------------------------------------------------------------
{

  albaVMEProber *prober;
  albaNEW(prober);
  
  int result = strcmp(prober->GetVisualPipe(), "albaPipeSurface");

  CPPUNIT_ASSERT(result==0);

  albaDEL(prober);
}

//----------------------------------------------------------------------------
void albaVMEProberTest::OutputSurfaceAcceptTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  albaVMEImage   *image;
  albaVMEVolumeGray  *volume;
  albaVMESurface *surface;

  albaNEW(image);
  albaNEW(volume);
  albaNEW(surface);

  CPPUNIT_ASSERT(prober->OutputSurfaceAccept(image)==NULL);
  CPPUNIT_ASSERT(prober->OutputSurfaceAccept(volume)==NULL);
  CPPUNIT_ASSERT(prober->OutputSurfaceAccept(surface)!=NULL);

  albaDEL(image);
  albaDEL(volume);
  albaDEL(surface);

  albaDEL(prober);
}

//----------------------------------------------------------------------------
void albaVMEProberTest::VolumeAcceptTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  albaVMEImage   *image;
  albaVMEVolumeGray  *volume;
  albaVMESurface *surface;

  albaNEW(image);
  albaNEW(volume);
  albaNEW(surface);

  CPPUNIT_ASSERT(prober->VolumeAccept(image)==NULL);
  CPPUNIT_ASSERT(prober->VolumeAccept(surface)==NULL);
  CPPUNIT_ASSERT(prober->VolumeAccept(volume)!=NULL);

  albaDEL(image);
  albaDEL(volume);
  albaDEL(surface);

  albaDEL(prober);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaVMEProber *newProber;
  albaNEW(prober);
  albaNEW(newProber);

  prober->SetModeToDistance();
  prober->SetDistanceModeToScalar();

  float maxDistance = 200.0;
  float distanceThreshold = maxDistance/2;
  prober->SetMaxDistance(maxDistance);
  prober->SetDistanceThreshold(distanceThreshold);

  newProber->DeepCopy(prober);

  CPPUNIT_ASSERT(newProber->GetMode() == albaDataPipeCustomProber::DISTANCE_MODE);
  CPPUNIT_ASSERT(newProber->GetDistanceMode() == albaDataPipeCustomProber::DISTANCE_MODE_SCALAR);
  CPPUNIT_ASSERT(newProber->GetMaxDistance() == maxDistance);
  CPPUNIT_ASSERT(newProber->GetDistanceThreshold() == distanceThreshold);

  albaDEL(prober);
  albaDEL(newProber);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::EqualsTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaVMEProber *newProber;
  albaNEW(prober);
  albaNEW(newProber);

  prober->SetModeToDistance();
  prober->SetDistanceModeToScalar();

  float maxDistance = 200.0;
  float distanceThreshold = maxDistance/2;
  prober->SetMaxDistance(maxDistance);
  prober->SetDistanceThreshold(distanceThreshold);

  newProber->SetModeToDistance();
  newProber->SetDistanceModeToScalar();
  newProber->SetMaxDistance(maxDistance);
  newProber->SetDistanceThreshold(distanceThreshold);

  CPPUNIT_ASSERT(newProber->Equals(prober));

  albaDEL(prober);
  albaDEL(newProber);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetSetDistanceThresholdTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  float threshold = 3.2;
  prober->SetDistanceThreshold(threshold);

  CPPUNIT_ASSERT(prober->GetDistanceThreshold() == threshold);

  albaDEL(prober);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetSetHighDensityTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  float highDensity = 25.0;
  prober->SetHighDensity(highDensity);

  CPPUNIT_ASSERT(prober->GetHighDensity() == highDensity);

  albaDEL(prober);
}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetSetLowDensityTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  float lowDensity = 0.1;
  prober->SetLowDensity(lowDensity);

  CPPUNIT_ASSERT(prober->GetLowDensity() == lowDensity);

  albaDEL(prober);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetSetMaxDistanceTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  float maxDistance = 200.0;
  prober->SetMaxDistance(maxDistance);

  CPPUNIT_ASSERT(prober->GetMaxDistance() == maxDistance);

  albaDEL(prober);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetSetModeTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  prober->SetModeToDensity();
  CPPUNIT_ASSERT(prober->GetMode() == albaDataPipeCustomProber::DENSITY_MODE);

  prober->SetModeToDistance();
  CPPUNIT_ASSERT(prober->GetMode() == albaDataPipeCustomProber::DISTANCE_MODE);

  prober->SetMode(0);
  CPPUNIT_ASSERT(prober->GetMode() == 0);

  prober->SetMode(1);
  CPPUNIT_ASSERT(prober->GetMode() == 1);

  albaDEL(prober);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetSetSurfaceLinkTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMEProber  *prober;
  albaVMESurface *surface;

  albaNEW(prober);
  albaNEW(surface);

  surface->ReparentTo(storage->GetRoot());
  prober->ReparentTo(storage->GetRoot());

  prober->SetSurfaceLink(surface);
  CPPUNIT_ASSERT(prober->GetSurfaceLink() == surface);

  prober->ReparentTo(NULL);
  surface->ReparentTo(NULL);

  albaDEL(surface);
  albaDEL(prober);
  albaDEL(storage);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetSetVolumeLinkTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMEProber      *prober;
  albaVMEVolumeGray  *volume;

  albaNEW(prober);
  albaNEW(volume);

  volume->ReparentTo(storage->GetRoot());
  prober->ReparentTo(storage->GetRoot());

  prober->SetVolumeLink(volume);
  albaVMEVolumeGray *linkedVolume = albaVMEVolumeGray::SafeDownCast(prober->GetVolumeLink());
  CPPUNIT_ASSERT(linkedVolume == volume);

  prober->ReparentTo(NULL);
  volume->ReparentTo(NULL);

  albaDEL(volume);
  albaDEL(prober);
  cppDEL(storage);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::GetSetDistanceModeTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  prober->SetDistanceModeToScalar();
  CPPUNIT_ASSERT(prober->GetDistanceMode() == albaDataPipeCustomProber::DISTANCE_MODE_SCALAR);

  prober->SetDistanceModeToVector();
  CPPUNIT_ASSERT(prober->GetDistanceMode() == albaDataPipeCustomProber::DISTANCE_MODE_VECTOR);

  albaDEL(prober);

}

//----------------------------------------------------------------------------
void albaVMEProberTest::IsAnimatedTest()
//----------------------------------------------------------------------------
{
  albaVMEProber *prober;
  albaNEW(prober);

  CPPUNIT_ASSERT(!prober->IsAnimated());

  albaDEL(prober);
}

//----------------------------------------------------------------------------
void albaVMEProberTest::IsDataAvailableTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMEProber      *prober;
  albaVMEVolumeGray  *volume;
  albaVMESurface     *surface;

  albaNEW(prober);
  albaNEW(volume);
  albaNEW(surface);

  volume->ReparentTo(storage->GetRoot());
  surface->ReparentTo(storage->GetRoot());
  prober->ReparentTo(storage->GetRoot());

  prober->SetVolumeLink(volume);
  prober->SetSurfaceLink(surface);

  CPPUNIT_ASSERT(prober->IsDataAvailable());

  prober->ReparentTo(NULL);
  volume->ReparentTo(NULL);
  surface->ReparentTo(NULL);

  albaDEL(volume);
  albaDEL(surface);
  albaDEL(prober);
  cppDEL(storage);


}