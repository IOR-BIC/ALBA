/*=========================================================================

 Program: MAF2
 Module: mafVMEMeterTest
 Authors: Eleonora Mambrini
 
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

#include <cppunit/config/SourcePrefix.h>

#include "mafVMEMeterTest.h"
#include "mafVMEMeter.h"

#include <wx/dir.h>

#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEOutput.h"

#include "vtkDataSet.h"
#include "mmaMeter.h"

#define TOLERANCE 1.0e-3


//----------------------------------------------------------------------------
void mafVMEMeterTest::setUp()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafVMEMeterTest::tearDown()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{

  mafVMEMeter *meter;
  mafNEW(meter);
  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetVisualPipeTest()
//----------------------------------------------------------------------------
{

  mafVMEMeter *meter;
  mafNEW(meter);

  int result = strcmp(meter->GetVisualPipe(), "mafPipeMeter");

  CPPUNIT_ASSERT(result==0);

  mafDEL(meter);
}

//----------------------------------------------------------------------------
void mafVMEMeterTest::VMEAcceptTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  mafVMEImage   *image;
  mafVMEVolumeGray  *volume;
  mafVMESurface *surface;

  mafNEW(image);
  mafNEW(volume);
  mafNEW(surface);

  CPPUNIT_ASSERT(meter->VolumeAccept(mafNode::SafeDownCast(image))!=NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(mafNode::SafeDownCast(surface))!=NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(mafNode::SafeDownCast(volume))!=NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(mafNode::SafeDownCast(NULL))==NULL);

  mafDEL(image);
  mafDEL(volume);
  mafDEL(surface);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::VolumeAcceptTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  mafVMEImage   *image;
  mafVMEVolumeGray  *volume;
  mafVMESurface *surface;

  mafNEW(image);
  mafNEW(volume);
  mafNEW(surface);

  CPPUNIT_ASSERT(meter->VolumeAccept(mafNode::SafeDownCast(image))==NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(mafNode::SafeDownCast(surface))==NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(mafNode::SafeDownCast(volume))!=NULL);

  mafDEL(image);
  mafDEL(volume);
  mafDEL(surface);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafVMEMeter *newMeter;
  mafNEW(meter);
  mafNEW(newMeter);

  //settings
  int deltaPercent = 5;
  double range[2];
  range[0] = 0.0;
  range[1] = 100.0;
  double initMeasure = 0.0;

  meter->SetDeltaPercent(deltaPercent);
  meter->SetDistanceRange(range);
  meter->SetInitMeasure(initMeasure);

  newMeter->DeepCopy(meter);

  double *newRange;

  CPPUNIT_ASSERT(newMeter->GetDeltaPercent() == deltaPercent);
  CPPUNIT_ASSERT(newMeter->GetInitMeasure() == initMeasure);
  newRange = newMeter->GetDistanceRange();
  CPPUNIT_ASSERT(newRange[0] == range[0] && newRange[1] == range[1]);

  mafDEL(meter);
  mafDEL(newMeter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::EqualsTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafVMEMeter *newMeter;
  mafNEW(meter);
  mafNEW(newMeter);

  //setting links
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMESurface *surface;
  mafNEW(surface);

  surface->SetParent(storage->GetRoot());
  meter->SetParent(storage->GetRoot());
  newMeter->SetParent(storage->GetRoot());

  meter->SetMeterLink("StartVME", mafNode::SafeDownCast(surface));
  newMeter->SetMeterLink("StartVME", mafNode::SafeDownCast(surface));
  CPPUNIT_ASSERT(newMeter->Equals(meter));

  mafVMESurface *surface2;
  mafNEW(surface2);

  surface2->SetParent(storage->GetRoot());
  meter->SetMeterLink("StartVME", mafNode::SafeDownCast(surface2));
  CPPUNIT_ASSERT(!newMeter->Equals(meter));

  meter->SetParent(NULL);
  newMeter->SetParent(NULL);
  surface->SetParent(NULL);
  surface2->SetParent(NULL);

  mafDEL(surface);
  mafDEL(surface2);

  mafDEL(meter);
  mafDEL(newMeter);
  mafDEL(storage);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::IsAnimatedTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  CPPUNIT_ASSERT(!meter->IsAnimated());

  mafDEL(meter);
}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetAngleTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();

  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->SetParent(root);
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->SetParent(root);
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND2;
  mafNEW(vmeParametricSurfaceEND2);	
  vmeParametricSurfaceEND2->SetParent(root);
  vmeParametricSurfaceEND2->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND2->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND2->Update();

  enum {X,Y,Z};
  mafMatrix *matrix1 = vmeParametricSurfaceEND1->GetOutput()->GetAbsMatrix();
  matrix1->SetElement(X,3,4); //set a translation value on x axis of 4.0
  matrix1->SetElement(Y,3,3); //set a translation value on x axis of 3.0

  mafMatrix *matrix2 = vmeParametricSurfaceEND2->GetOutput()->GetAbsMatrix();
  matrix2->SetElement(X,3,3); //set a translation value on x axis of 4.0
  matrix2->SetElement(Y,3,-4); //set a translation value on x axis of 3.0

  mafVMEMeter *meter;
  mafNEW(meter);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
  meter->SetParent(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->SetMeterMode(mafVMEMeter::LINE_ANGLE);
  meter->Modified();
  meter->Update();
  meter->GetOutput()->GetVTKData()->Update();

  meter->GetOutput()->Update();
  double angle = meter->GetAngle();
  CPPUNIT_ASSERT((angle - 90) < TOLERANCE);

  meter->SetParent(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  vmeParametricSurfaceEND1->SetParent(NULL);
  vmeParametricSurfaceEND2->SetParent(NULL);

  mafDEL(meter);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);
  mafDEL(vmeParametricSurfaceEND2);


  mafDEL(storage);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetDistanceTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();

  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->SetParent(root);
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->SetParent(root);
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND2;
  mafNEW(vmeParametricSurfaceEND2);	
  vmeParametricSurfaceEND2->SetParent(root);
  vmeParametricSurfaceEND2->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND2->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND2->Update();

  enum {X,Y,Z};
  mafMatrix *matrix1 = vmeParametricSurfaceEND1->GetOutput()->GetAbsMatrix();
  matrix1->SetElement(X,3,4); //set a translation value on x axis of 4.0
  matrix1->SetElement(Y,3,3); //set a translation value on x axis of 3.0

  mafMatrix *matrix2 = vmeParametricSurfaceEND2->GetOutput()->GetAbsMatrix();
  matrix2->SetElement(X,3,3); //set a translation value on x axis of 4.0
  matrix2->SetElement(Y,3,-4); //set a translation value on x axis of 3.0

  mafVMEMeter *meter;
  mafNEW(meter);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
  meter->SetParent(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->GetVTKData()->Update();

  meter->GetOutput()->Update();

  double distance = meter->GetDistance();
  //test here
  CPPUNIT_ASSERT(distance == 5);

  meter->SetParent(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  vmeParametricSurfaceEND1->SetParent(NULL);
  vmeParametricSurfaceEND2->SetParent(NULL);

  mafDEL(meter);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);
  mafDEL(vmeParametricSurfaceEND2);


  mafDEL(storage);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetEndPoint2CoordinateTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();

  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->SetParent(root);
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->SetParent(root);
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND2;
  mafNEW(vmeParametricSurfaceEND2);	
  vmeParametricSurfaceEND2->SetParent(root);
  vmeParametricSurfaceEND2->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND2->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND2->Update();

  double pos[3];
  pos[0] = 4;
  pos[1] = 3;
  pos[2] = 0;
  mafMatrix *matrix = vmeParametricSurfaceEND2->GetOutput()->GetAbsMatrix();
  matrix->SetElement(0,3,pos[0]); //set a translation value on x axis of 4.0
  matrix->SetElement(1,3,pos[1]); //set a translation value on y axis of 3.0
  matrix->SetElement(2,3,pos[2]); //set a translation value on z axis of 3.0

  mafVMEMeter *meter;
  mafNEW(meter);
  meter->SetMeterMode(mafVMEMeter::LINE_ANGLE);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
  meter->SetParent(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->GetVTKData()->Update();
  meter->GetOutput()->Update();

  double *outPos;
  outPos = meter->GetEndPoint2Coordinate();
  CPPUNIT_ASSERT(outPos[0] == pos[0] && outPos[1] == pos[1] && outPos[2] == pos[2]);

  meter->SetParent(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  vmeParametricSurfaceEND1->SetParent(NULL);
  vmeParametricSurfaceEND2->SetParent(NULL);

  mafDEL(meter);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);
  mafDEL(vmeParametricSurfaceEND2);

  mafDEL(storage);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetEndPointCoordinateTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();

  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->SetParent(root);
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->SetParent(root);
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  double pos[3];
  pos[0] = 4;
  pos[1] = 3;
  pos[2] = 0;
  mafMatrix *matrix = vmeParametricSurfaceEND1->GetOutput()->GetAbsMatrix();
  matrix->SetElement(0,3,pos[0]); //set a translation value on x axis of 4.0
  matrix->SetElement(1,3,pos[1]); //set a translation value on y axis of 3.0
  matrix->SetElement(2,3,pos[2]); //set a translation value on z axis of 3.0

  mafVMEMeter *meter;
  mafNEW(meter);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetParent(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->GetVTKData()->Update();
  meter->GetOutput()->Update();

  double *outPos;
  outPos = meter->GetEndPointCoordinate();
  CPPUNIT_ASSERT(outPos[0] == pos[0] && outPos[1] == pos[1] && outPos[2] == pos[2]);

  meter->SetParent(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  vmeParametricSurfaceEND1->SetParent(NULL);

  mafDEL(meter);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);

  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetMeterAttributesTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  mmaMeter *attributes = meter->GetMeterAttributes();

  //settings
  int deltaPercent = 5;
  double range[2];
  range[0] = 0.0;
  range[1] = 100.0;
  double initMeasure = 0.0;

  meter->SetInitMeasure(0.0);
  meter->SetDeltaPercent(deltaPercent);
  meter->SetMeterCappingOff();
  meter->SetDistanceRange(range);

  CPPUNIT_ASSERT(attributes->m_DeltaPercent == deltaPercent &&
                 attributes->m_DistanceRange[0] == range[0] &&
                 attributes->m_DistanceRange[1] == range[1] &&
                 attributes->m_InitMeasure == initMeasure &&
                 attributes->m_Capping == 0);

  mafDEL(meter);
}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetPolylineOutputTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();

  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->SetParent(root);
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->SetParent(root);
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND2;
  mafNEW(vmeParametricSurfaceEND2);	
  vmeParametricSurfaceEND2->SetParent(root);
  vmeParametricSurfaceEND2->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND2->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND2->Update();

  enum {X,Y,Z};
  mafMatrix *matrix1 = vmeParametricSurfaceEND1->GetOutput()->GetAbsMatrix();
  matrix1->SetElement(X,3,4); //set a translation value on x axis of 4.0
  matrix1->SetElement(Y,3,3); //set a translation value on x axis of 3.0

  mafMatrix *matrix2 = vmeParametricSurfaceEND2->GetOutput()->GetAbsMatrix();
  matrix2->SetElement(X,3,3); //set a translation value on x axis of 4.0
  matrix2->SetElement(Y,3,-4); //set a translation value on x axis of 3.0

  mafVMEMeter *meter;
  mafNEW(meter);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
  meter->SetParent(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->GetVTKData()->Update();

  meter->GetOutput()->Update();

  CPPUNIT_ASSERT(mafVMEOutputPolyline::SafeDownCast(meter->GetPolylineOutput()));

  meter->SetParent(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  vmeParametricSurfaceEND1->SetParent(NULL);
  vmeParametricSurfaceEND2->SetParent(NULL);

  mafDEL(meter);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);
  mafDEL(vmeParametricSurfaceEND2);


  mafDEL(storage);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetDeltaPercentTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  int deltaPercent = 5;

  meter->SetDeltaPercent(deltaPercent);

  CPPUNIT_ASSERT(meter->GetDeltaPercent()==deltaPercent);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetDistanceRangeTest()
//----------------------------------------------------------------------------
{

  mafVMEMeter *meter;
  mafNEW(meter);

  double distanceRange[2];
  distanceRange[0] = 0.0;
  distanceRange[1] = 100.0;

  meter->SetDistanceRange(distanceRange);

  double *rangeOut;

  rangeOut = meter->GetDistanceRange();

  CPPUNIT_ASSERT(rangeOut[0] == distanceRange[0] && rangeOut[1] == distanceRange[1]);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetGenerateEventTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  meter->SetGenerateEventOn();
  CPPUNIT_ASSERT(meter->GetGenerateEvent());

  meter->SetGenerateEventOff();
  CPPUNIT_ASSERT(!meter->GetGenerateEvent());

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetInitMeasureTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  double initMeasure = 1.0;
  meter->SetInitMeasure(initMeasure);

  CPPUNIT_ASSERT(meter->GetInitMeasure() == initMeasure);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetMeterCappingTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  meter->SetMeterCappingOn();
  CPPUNIT_ASSERT(meter->GetMeterCapping());

  meter->SetMeterCappingOff();
  CPPUNIT_ASSERT(!meter->GetMeterCapping());

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetMeterColorModeTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  meter->SetMeterColorModeToOneColor();
  CPPUNIT_ASSERT(meter->GetMeterColorMode() == mafVMEMeter::ONE_COLOR);

  meter->SetMeterColorModeToRangeColor();
  CPPUNIT_ASSERT(meter->GetMeterColorMode() == mafVMEMeter::RANGE_COLOR);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetMeterLinkTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMESurface *surface, *surface2, *surface3;
  mafNEW(surface);
  mafNEW(surface2);
  mafNEW(surface3);

  surface->SetParent(storage->GetRoot());
  surface2->SetParent(storage->GetRoot());
  surface3->SetParent(storage->GetRoot());
  meter->SetParent(storage->GetRoot());

  meter->SetMeterLink("StartVME", mafNode::SafeDownCast(surface));
  CPPUNIT_ASSERT(meter->GetLink("StartVME") == mafNode::SafeDownCast(surface));
  CPPUNIT_ASSERT(meter->GetStartVME() == mafNode::SafeDownCast(surface));
 
  meter->SetMeterLink("EndVME1", mafNode::SafeDownCast(surface2));
  CPPUNIT_ASSERT(meter->GetLink("EndVME1") == mafNode::SafeDownCast(surface2));
  CPPUNIT_ASSERT(meter->GetEnd1VME() == mafNode::SafeDownCast(surface2));

  meter->SetMeterLink("EndVME2", mafNode::SafeDownCast(surface3));
  CPPUNIT_ASSERT(meter->GetLink("EndVME2") == mafNode::SafeDownCast(surface3));
  CPPUNIT_ASSERT(meter->GetEnd2VME() == mafNode::SafeDownCast(surface3));

  meter->SetParent(NULL);
  
  surface->SetParent(NULL);
  surface2->SetParent(NULL);
  surface3->SetParent(NULL);
  
  mafDEL(surface);
  mafDEL(surface2);
  mafDEL(surface3);
  mafDEL(meter);
  mafDEL(storage);
}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetMeterMeasureType()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  meter->SetMeterMeasureType(mafVMEMeter::ABSOLUTE_MEASURE);
  CPPUNIT_ASSERT(meter->GetMeterMeasureType() == mafVMEMeter::ABSOLUTE_MEASURE);

  meter->SetMeterMeasureType(mafVMEMeter::RELATIVE_MEASURE);
  CPPUNIT_ASSERT(meter->GetMeterMeasureType() == mafVMEMeter::RELATIVE_MEASURE);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetMeterModeTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  meter->SetMeterModeToLineAngle();
  CPPUNIT_ASSERT(meter->GetMeterMode() == mafVMEMeter::LINE_ANGLE);

  meter->SetMeterModeToLineDistance();
  CPPUNIT_ASSERT(meter->GetMeterMode() == mafVMEMeter::LINE_DISTANCE);

  meter->SetMeterModeToPointDistance();
  CPPUNIT_ASSERT(meter->GetMeterMode() == mafVMEMeter::POINT_DISTANCE);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetMeterRadiusTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  double radius = 0.5;
  meter->SetMeterRadius(radius);
  CPPUNIT_ASSERT(meter->GetMeterRadius() == radius);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetSetMeterRepresentationTest()
//----------------------------------------------------------------------------
{
  mafVMEMeter *meter;
  mafNEW(meter);

  meter->SetMeterRepresentationToLine();
  CPPUNIT_ASSERT(meter->GetMeterRepresentation() == mafVMEMeter::LINE_REPRESENTATION);

  meter->SetMeterRepresentationToTube();
  CPPUNIT_ASSERT(meter->GetMeterRepresentation() == mafVMEMeter::TUBE_REPRESENTATION);

  mafDEL(meter);

}

//----------------------------------------------------------------------------
void mafVMEMeterTest::GetStartPointCoordinateTest()
//----------------------------------------------------------------------------
{
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafVMERoot *root=storage->GetRoot();

  //create a parametric surface
  mafVMESurfaceParametric *vmeParametricSurfaceSTART;
  mafNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->SetParent(root);
  vmeParametricSurfaceSTART->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceSTART->SetParent(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  mafVMESurfaceParametric *vmeParametricSurfaceEND1;
  mafNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->SetParent(root);
  vmeParametricSurfaceEND1->GetOutput()->GetVTKData()->Update();
  vmeParametricSurfaceEND1->SetParent(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  double pos[3];
  pos[0] = 4;
  pos[1] = 3;
  pos[2] = 0;
  mafMatrix *matrix = vmeParametricSurfaceSTART->GetOutput()->GetAbsMatrix();
  matrix->SetElement(0,3,pos[0]); //set a translation value on x axis of 4.0
  matrix->SetElement(1,3,pos[1]); //set a translation value on y axis of 3.0
  matrix->SetElement(2,3,pos[2]); //set a translation value on z axis of 3.0

  mafVMEMeter *meter;
  mafNEW(meter);

  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetParent(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->GetVTKData()->Update();
  meter->GetOutput()->Update();

  double *outPos;
  outPos = meter->GetStartPointCoordinate();
  CPPUNIT_ASSERT(outPos[0] == pos[0] && outPos[1] == pos[1] && outPos[2] == pos[2]);

  meter->SetParent(NULL);
  vmeParametricSurfaceSTART->SetParent(NULL);
  vmeParametricSurfaceEND1->SetParent(NULL);

  mafDEL(meter);
  mafDEL(vmeParametricSurfaceSTART);
  mafDEL(vmeParametricSurfaceEND1);

  mafDEL(storage);

}