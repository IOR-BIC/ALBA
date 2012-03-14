/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEProberTest.cpp,v $
Language:  C++
Date:      $Date: 2010-02-22 16:56:37 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
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

#include "mafVMEProberTest.h"
#include "mafVMEProber.h"

#include <wx/dir.h>

#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafDataPipeCustomProber.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEOutput.h"

#include "vtkDataSet.h"


//----------------------------------------------------------------------------
void mafVMEProberTest::setUp()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMEProberTest::tearDown()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafVMEProberTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{

  mafVMEProber *prober;
  mafNEW(prober);
  mafDEL(prober);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetVisualPipeTest()
//----------------------------------------------------------------------------
{

  mafVMEProber *prober;
  mafNEW(prober);
  
  int result = strcmp(prober->GetVisualPipe(), "mafPipeSurface");

  CPPUNIT_ASSERT(result==0);

  mafDEL(prober);
}

//----------------------------------------------------------------------------
void mafVMEProberTest::OutputSurfaceAcceptTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  mafVMEImage   *image;
  mafVMEVolumeGray  *volume;
  mafVMESurface *surface;

  mafNEW(image);
  mafNEW(volume);
  mafNEW(surface);

  CPPUNIT_ASSERT(prober->OutputSurfaceAccept(mafNode::SafeDownCast(image))==NULL);
  CPPUNIT_ASSERT(prober->OutputSurfaceAccept(mafNode::SafeDownCast(volume))==NULL);
  CPPUNIT_ASSERT(prober->OutputSurfaceAccept(mafNode::SafeDownCast(surface))!=NULL);

  mafDEL(image);
  mafDEL(volume);
  mafDEL(surface);

  mafDEL(prober);
}

//----------------------------------------------------------------------------
void mafVMEProberTest::VolumeAcceptTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  mafVMEImage   *image;
  mafVMEVolumeGray  *volume;
  mafVMESurface *surface;

  mafNEW(image);
  mafNEW(volume);
  mafNEW(surface);

  CPPUNIT_ASSERT(prober->VolumeAccept(mafNode::SafeDownCast(image))==NULL);
  CPPUNIT_ASSERT(prober->VolumeAccept(mafNode::SafeDownCast(surface))==NULL);
  CPPUNIT_ASSERT(prober->VolumeAccept(mafNode::SafeDownCast(volume))!=NULL);

  mafDEL(image);
  mafDEL(volume);
  mafDEL(surface);

  mafDEL(prober);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafVMEProber *newProber;
  mafNEW(prober);
  mafNEW(newProber);

  prober->SetModeToDistance();
  prober->SetDistanceModeToScalar();

  float maxDistance = 200.0;
  float distanceThreshold = maxDistance/2;
  prober->SetMaxDistance(maxDistance);
  prober->SetDistanceThreshold(distanceThreshold);

  newProber->DeepCopy(prober);

  CPPUNIT_ASSERT(newProber->GetMode() == mafDataPipeCustomProber::DISTANCE_MODE);
  CPPUNIT_ASSERT(newProber->GetDistanceMode() == mafDataPipeCustomProber::DISTANCE_MODE_SCALAR);
  CPPUNIT_ASSERT(newProber->GetMaxDistance() == maxDistance);
  CPPUNIT_ASSERT(newProber->GetDistanceThreshold() == distanceThreshold);

  mafDEL(prober);
  mafDEL(newProber);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::EqualsTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafVMEProber *newProber;
  mafNEW(prober);
  mafNEW(newProber);

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

  mafDEL(prober);
  mafDEL(newProber);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetSetDistanceThresholdTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  float threshold = 3.2;
  prober->SetDistanceThreshold(threshold);

  CPPUNIT_ASSERT(prober->GetDistanceThreshold() == threshold);

  mafDEL(prober);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetSetHighDensityTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  float highDensity = 25.0;
  prober->SetHighDensity(highDensity);

  CPPUNIT_ASSERT(prober->GetHighDensity() == highDensity);

  mafDEL(prober);
}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetSetLowDensityTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  float lowDensity = 0.1;
  prober->SetLowDensity(lowDensity);

  CPPUNIT_ASSERT(prober->GetLowDensity() == lowDensity);

  mafDEL(prober);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetSetMaxDistanceTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  float maxDistance = 200.0;
  prober->SetMaxDistance(maxDistance);

  CPPUNIT_ASSERT(prober->GetMaxDistance() == maxDistance);

  mafDEL(prober);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetSetModeTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  prober->SetModeToDensity();
  CPPUNIT_ASSERT(prober->GetMode() == mafDataPipeCustomProber::DENSITY_MODE);

  prober->SetModeToDistance();
  CPPUNIT_ASSERT(prober->GetMode() == mafDataPipeCustomProber::DISTANCE_MODE);

  prober->SetMode(0);
  CPPUNIT_ASSERT(prober->GetMode() == 0);

  prober->SetMode(1);
  CPPUNIT_ASSERT(prober->GetMode() == 1);

  mafDEL(prober);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetSetSurfaceLinkTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMEProber  *prober;
  mafVMESurface *surface;

  mafNEW(prober);
  mafNEW(surface);

  surface->SetParent(storage->GetRoot());
  prober->SetParent(storage->GetRoot());

  prober->SetSurfaceLink(mafNode::SafeDownCast(surface));
  CPPUNIT_ASSERT(prober->GetSurfaceLink() == surface);

  prober->SetParent(NULL);
  surface->SetParent(NULL);

  mafDEL(surface);
  mafDEL(prober);
  mafDEL(storage);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetSetVolumeLinkTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMEProber      *prober;
  mafVMEVolumeGray  *volume;

  mafNEW(prober);
  mafNEW(volume);

  volume->SetParent(storage->GetRoot());
  prober->SetParent(storage->GetRoot());

  prober->SetVolumeLink(mafNode::SafeDownCast(volume));
  mafVMEVolumeGray *linkedVolume = mafVMEVolumeGray::SafeDownCast(prober->GetVolumeLink());
  CPPUNIT_ASSERT(linkedVolume == volume);

  prober->SetParent(NULL);
  volume->SetParent(NULL);

  mafDEL(volume);
  mafDEL(prober);
  cppDEL(storage);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::GetSetDistanceModeTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  prober->SetDistanceModeToScalar();
  CPPUNIT_ASSERT(prober->GetDistanceMode() == mafDataPipeCustomProber::DISTANCE_MODE_SCALAR);

  prober->SetDistanceModeToVector();
  CPPUNIT_ASSERT(prober->GetDistanceMode() == mafDataPipeCustomProber::DISTANCE_MODE_VECTOR);

  mafDEL(prober);

}

//----------------------------------------------------------------------------
void mafVMEProberTest::IsAnimatedTest()
//----------------------------------------------------------------------------
{
  mafVMEProber *prober;
  mafNEW(prober);

  CPPUNIT_ASSERT(!prober->IsAnimated());

  mafDEL(prober);
}

//----------------------------------------------------------------------------
void mafVMEProberTest::IsDataAvailableTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMEProber      *prober;
  mafVMEVolumeGray  *volume;
  mafVMESurface     *surface;

  mafNEW(prober);
  mafNEW(volume);
  mafNEW(surface);

  volume->SetParent(storage->GetRoot());
  surface->SetParent(storage->GetRoot());
  prober->SetParent(storage->GetRoot());

  prober->SetVolumeLink(mafNode::SafeDownCast(volume));
  prober->SetSurfaceLink(mafNode::SafeDownCast(surface));

  CPPUNIT_ASSERT(prober->IsDataAvailable());

  prober->SetParent(NULL);
  volume->SetParent(NULL);
  surface->SetParent(NULL);

  mafDEL(volume);
  mafDEL(surface);
  mafDEL(prober);
  cppDEL(storage);


}