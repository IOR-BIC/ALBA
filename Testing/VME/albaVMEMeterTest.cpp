/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeterTest
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

#include "albaVMEMeterTest.h"
#include "albaVMEMeter.h"

#include <wx/dir.h>

#include "albaVMEImage.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEOutput.h"

#include "vtkDataSet.h"
#include "mmaMeter.h"

#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEMeterTest::DynamicAllocationTest()
//----------------------------------------------------------------------------
{

  albaVMEMeter *meter;
  albaNEW(meter);
  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetVisualPipeTest()
//----------------------------------------------------------------------------
{

  albaVMEMeter *meter;
  albaNEW(meter);

  int result = strcmp(meter->GetVisualPipe(), "albaPipeMeter");

  CPPUNIT_ASSERT(result==0);

  albaDEL(meter);
}

//----------------------------------------------------------------------------
void albaVMEMeterTest::VMEAcceptTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  albaVMEImage   *image;
  albaVMEVolumeGray  *volume;
  albaVMESurface *surface;

  albaNEW(image);
  albaNEW(volume);
  albaNEW(surface);

  CPPUNIT_ASSERT(meter->VolumeAccept(image)!=NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(surface)!=NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(volume)!=NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(NULL)==NULL);

  albaDEL(image);
  albaDEL(volume);
  albaDEL(surface);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::VolumeAcceptTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  albaVMEImage   *image;
  albaVMEVolumeGray  *volume;
  albaVMESurface *surface;

  albaNEW(image);
  albaNEW(volume);
  albaNEW(surface);

  CPPUNIT_ASSERT(meter->VolumeAccept(image)==NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(surface)==NULL);
  CPPUNIT_ASSERT(meter->VolumeAccept(volume)!=NULL);

  albaDEL(image);
  albaDEL(volume);
  albaDEL(surface);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::DeepCopyTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaVMEMeter *newMeter;
  albaNEW(meter);
  albaNEW(newMeter);

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

  albaDEL(meter);
  albaDEL(newMeter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::EqualsTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaVMEMeter *newMeter;
  albaNEW(meter);
  albaNEW(newMeter);

  //setting links
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMESurface *surface;
  albaNEW(surface);

  surface->ReparentTo(storage->GetRoot());
  meter->ReparentTo(storage->GetRoot());
  newMeter->ReparentTo(storage->GetRoot());

  meter->SetMeterLink("StartVME", surface);
  newMeter->SetMeterLink("StartVME", surface);
  CPPUNIT_ASSERT(newMeter->Equals(meter));

  albaVMESurface *surface2;
  albaNEW(surface2);

  surface2->ReparentTo(storage->GetRoot());
  meter->SetMeterLink("StartVME", surface2);
  CPPUNIT_ASSERT(!newMeter->Equals(meter));

  meter->ReparentTo(NULL);
  newMeter->ReparentTo(NULL);
  surface->ReparentTo(NULL);
  surface2->ReparentTo(NULL);

  albaDEL(surface);
  albaDEL(surface2);

  albaDEL(meter);
  albaDEL(newMeter);
  albaDEL(storage);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::IsAnimatedTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  CPPUNIT_ASSERT(!meter->IsAnimated());

  albaDEL(meter);
}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetAngleTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();

  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->ReparentTo(root);
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(root);
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND2;
  albaNEW(vmeParametricSurfaceEND2);	
  vmeParametricSurfaceEND2->ReparentTo(root);
  vmeParametricSurfaceEND2->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND2->Update();

  enum {X,Y,Z};
  albaMatrix matrix1;
  matrix1.SetElement(X,3,4); //set a translation value on x axis of 4.0
  matrix1.SetElement(Y,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix1);

  albaMatrix matrix2;
  matrix2.SetElement(X,3,3); //set a translation value on x axis of 4.0
  matrix2.SetElement(Y,3,-4); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND2->SetAbsMatrix(matrix2);

  albaVMEMeter *meter;
  albaNEW(meter);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
  meter->ReparentTo(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->SetMeterMode(albaVMEMeter::LINE_ANGLE);
  meter->Modified();
  meter->Update();

  meter->GetOutput()->Update();
  double angle = meter->GetAngle();
  CPPUNIT_ASSERT((angle - 90) < TOLERANCE);

  meter->ReparentTo(NULL);
  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);
  vmeParametricSurfaceEND2->ReparentTo(NULL);

  albaDEL(meter);
  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);
  albaDEL(vmeParametricSurfaceEND2);


  albaDEL(storage);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetDistanceTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();

  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);
	vmeParametricSurfaceSTART->ReparentTo(root);
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(root);
  vmeParametricSurfaceEND1->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND2;
  albaNEW(vmeParametricSurfaceEND2);	
  vmeParametricSurfaceEND2->ReparentTo(root);
  vmeParametricSurfaceEND2->Update();

  enum {X,Y,Z};
  albaMatrix matrix1;
  matrix1.SetElement(X,3,4); //set a translation value on x axis of 4.0
  matrix1.SetElement(Y,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix1);

  albaMatrix matrix2;
  matrix2.SetElement(X,3,3); //set a translation value on x axis of 4.0
  matrix2.SetElement(Y,3,-4); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND2->SetAbsMatrix(matrix2);

  albaVMEMeter *meter;
  albaNEW(meter);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
  meter->ReparentTo(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->Update();

  double distance = meter->GetDistance();
  //test here
  CPPUNIT_ASSERT(distance == 5);

  meter->ReparentTo(NULL);
  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);
  vmeParametricSurfaceEND2->ReparentTo(NULL);

  albaDEL(meter);
  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);
  albaDEL(vmeParametricSurfaceEND2);


  albaDEL(storage);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetEndPoint2CoordinateTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();

  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->ReparentTo(root);
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(root);
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND2;
  albaNEW(vmeParametricSurfaceEND2);	
  vmeParametricSurfaceEND2->ReparentTo(root);
  vmeParametricSurfaceEND2->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND2->Update();

  double pos[3];
  pos[0] = 4;
  pos[1] = 3;
  pos[2] = 0;
  albaMatrix matrix;
  matrix.SetElement(0,3,pos[0]); //set a translation value on x axis of 4.0
  matrix.SetElement(1,3,pos[1]); //set a translation value on y axis of 3.0
  matrix.SetElement(2,3,pos[2]); //set a translation value on z axis of 3.0
	vmeParametricSurfaceEND2->SetAbsMatrix(matrix);

  albaVMEMeter *meter;
  albaNEW(meter);
  meter->SetMeterMode(albaVMEMeter::LINE_ANGLE);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
  meter->ReparentTo(storage->GetRoot());
  meter->Modified();
  meter->Update();
  meter->GetOutput()->Update();

  double *outPos;
  outPos = meter->GetEndPoint2Coordinate();
  CPPUNIT_ASSERT(outPos[0] == pos[0] && outPos[1] == pos[1] && outPos[2] == pos[2]);

  meter->ReparentTo(NULL);
  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);
  vmeParametricSurfaceEND2->ReparentTo(NULL);

  albaDEL(meter);
  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);
  albaDEL(vmeParametricSurfaceEND2);

  albaDEL(storage);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetEndPointCoordinateTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();

  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->ReparentTo(root);
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(root);
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  double pos[3];
  pos[0] = 4;
  pos[1] = 3;
  pos[2] = 0;
	albaMatrix matrix;
  matrix.SetElement(0,3,pos[0]); //set a translation value on x axis of 4.0
  matrix.SetElement(1,3,pos[1]); //set a translation value on y axis of 3.0
  matrix.SetElement(2,3,pos[2]); //set a translation value on z axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix);

  albaVMEMeter *meter;
  albaNEW(meter);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->ReparentTo(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->Update();

  double *outPos;
  outPos = meter->GetEndPointCoordinate();
  CPPUNIT_ASSERT(outPos[0] == pos[0] && outPos[1] == pos[1] && outPos[2] == pos[2]);

  meter->ReparentTo(NULL);
  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);

  albaDEL(meter);
  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);

  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetMeterAttributesTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

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

  albaDEL(meter);
}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetPolylineOutputTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();

  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->ReparentTo(root);
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(root);
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND2;
  albaNEW(vmeParametricSurfaceEND2);	
  vmeParametricSurfaceEND2->ReparentTo(root);
  vmeParametricSurfaceEND2->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND2->Update();

  enum {X,Y,Z};
  albaMatrix matrix1;
  matrix1.SetElement(X,3,4); //set a translation value on x axis of 4.0
  matrix1.SetElement(Y,3,3); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND1->SetAbsMatrix(matrix1);

  albaMatrix matrix2;
  matrix1.SetElement(X,3,3); //set a translation value on x axis of 4.0
  matrix2.SetElement(Y,3,-4); //set a translation value on x axis of 3.0
	vmeParametricSurfaceEND2->SetAbsMatrix(matrix2);

  albaVMEMeter *meter;
  albaNEW(meter);
  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->SetMeterLink("EndVME2",vmeParametricSurfaceEND2);
  meter->ReparentTo(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->Update();

  CPPUNIT_ASSERT(albaVMEOutputPolyline::SafeDownCast(meter->GetPolylineOutput()));

  meter->ReparentTo(NULL);
  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);
  vmeParametricSurfaceEND2->ReparentTo(NULL);

  albaDEL(meter);
  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);
  albaDEL(vmeParametricSurfaceEND2);


  albaDEL(storage);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetDeltaPercentTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  int deltaPercent = 5;

  meter->SetDeltaPercent(deltaPercent);

  CPPUNIT_ASSERT(meter->GetDeltaPercent()==deltaPercent);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetDistanceRangeTest()
//----------------------------------------------------------------------------
{

  albaVMEMeter *meter;
  albaNEW(meter);

  double distanceRange[2];
  distanceRange[0] = 0.0;
  distanceRange[1] = 100.0;

  meter->SetDistanceRange(distanceRange);

  double *rangeOut;

  rangeOut = meter->GetDistanceRange();

  CPPUNIT_ASSERT(rangeOut[0] == distanceRange[0] && rangeOut[1] == distanceRange[1]);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetGenerateEventTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  meter->SetGenerateEventOn();
  CPPUNIT_ASSERT(meter->GetGenerateEvent());

  meter->SetGenerateEventOff();
  CPPUNIT_ASSERT(!meter->GetGenerateEvent());

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetInitMeasureTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  double initMeasure = 1.0;
  meter->SetInitMeasure(initMeasure);

  CPPUNIT_ASSERT(meter->GetInitMeasure() == initMeasure);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetMeterCappingTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  meter->SetMeterCappingOn();
  CPPUNIT_ASSERT(meter->GetMeterCapping());

  meter->SetMeterCappingOff();
  CPPUNIT_ASSERT(!meter->GetMeterCapping());

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetMeterColorModeTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  meter->SetMeterColorModeToOneColor();
  CPPUNIT_ASSERT(meter->GetMeterColorMode() == albaVMEMeter::ONE_COLOR);

  meter->SetMeterColorModeToRangeColor();
  CPPUNIT_ASSERT(meter->GetMeterColorMode() == albaVMEMeter::RANGE_COLOR);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetMeterLinkTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMESurface *surface, *surface2, *surface3;
  albaNEW(surface);
  albaNEW(surface2);
  albaNEW(surface3);

  surface->ReparentTo(storage->GetRoot());
  surface2->ReparentTo(storage->GetRoot());
  surface3->ReparentTo(storage->GetRoot());
  meter->ReparentTo(storage->GetRoot());

  meter->SetMeterLink("StartVME", surface);
  CPPUNIT_ASSERT(meter->GetLink("StartVME") == surface);
  CPPUNIT_ASSERT(meter->GetStartVME() == surface);
 
  meter->SetMeterLink("EndVME1", surface2);
  CPPUNIT_ASSERT(meter->GetLink("EndVME1") == surface2);
  CPPUNIT_ASSERT(meter->GetEnd1VME() == surface2);

  meter->SetMeterLink("EndVME2", surface3);
  CPPUNIT_ASSERT(meter->GetLink("EndVME2") == surface3);
  CPPUNIT_ASSERT(meter->GetEnd2VME() == surface3);

  meter->ReparentTo(NULL);
  
  surface->ReparentTo(NULL);
  surface2->ReparentTo(NULL);
  surface3->ReparentTo(NULL);
  
  albaDEL(surface);
  albaDEL(surface2);
  albaDEL(surface3);
  albaDEL(meter);
  albaDEL(storage);
}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetMeterMeasureType()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  meter->SetMeterMeasureType(albaVMEMeter::ABSOLUTE_MEASURE);
  CPPUNIT_ASSERT(meter->GetMeterMeasureType() == albaVMEMeter::ABSOLUTE_MEASURE);

  meter->SetMeterMeasureType(albaVMEMeter::RELATIVE_MEASURE);
  CPPUNIT_ASSERT(meter->GetMeterMeasureType() == albaVMEMeter::RELATIVE_MEASURE);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetMeterModeTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  meter->SetMeterModeToLineAngle();
  CPPUNIT_ASSERT(meter->GetMeterMode() == albaVMEMeter::LINE_ANGLE);

  meter->SetMeterModeToLineDistance();
  CPPUNIT_ASSERT(meter->GetMeterMode() == albaVMEMeter::LINE_DISTANCE);

  meter->SetMeterModeToPointDistance();
  CPPUNIT_ASSERT(meter->GetMeterMode() == albaVMEMeter::POINT_DISTANCE);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetMeterRadiusTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  double radius = 0.5;
  meter->SetMeterRadius(radius);
  CPPUNIT_ASSERT(meter->GetMeterRadius() == radius);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetSetMeterRepresentationTest()
//----------------------------------------------------------------------------
{
  albaVMEMeter *meter;
  albaNEW(meter);

  meter->SetMeterRepresentationToLine();
  CPPUNIT_ASSERT(meter->GetMeterRepresentation() == albaVMEMeter::LINE_REPRESENTATION);

  meter->SetMeterRepresentationToTube();
  CPPUNIT_ASSERT(meter->GetMeterRepresentation() == albaVMEMeter::TUBE_REPRESENTATION);

  albaDEL(meter);

}

//----------------------------------------------------------------------------
void albaVMEMeterTest::GetStartPointCoordinateTest()
//----------------------------------------------------------------------------
{
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaVMERoot *root=storage->GetRoot();

  //create a parametric surface
  albaVMESurfaceParametric *vmeParametricSurfaceSTART;
  albaNEW(vmeParametricSurfaceSTART);
  vmeParametricSurfaceSTART->ReparentTo(root);
  vmeParametricSurfaceSTART->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceSTART->Update();

  albaVMESurfaceParametric *vmeParametricSurfaceEND1;
  albaNEW(vmeParametricSurfaceEND1);	
  vmeParametricSurfaceEND1->ReparentTo(root);
  vmeParametricSurfaceEND1->ReparentTo(storage->GetRoot());
  vmeParametricSurfaceEND1->Update();

  double pos[3];
  pos[0] = 4;
  pos[1] = 3;
  pos[2] = 0;
  albaMatrix matrix;
  matrix.SetElement(0,3,pos[0]); //set a translation value on x axis of 4.0
  matrix.SetElement(1,3,pos[1]); //set a translation value on y axis of 3.0
  matrix.SetElement(2,3,pos[2]); //set a translation value on z axis of 3.0
	vmeParametricSurfaceSTART->SetAbsMatrix(matrix);

  albaVMEMeter *meter;
  albaNEW(meter);

  meter->SetMeterLink("StartVME",vmeParametricSurfaceSTART);
  meter->SetMeterLink("EndVME1",vmeParametricSurfaceEND1);
  meter->ReparentTo(storage->GetRoot());
  meter->Modified();
  meter->Update();

  meter->GetOutput()->Update();

  double *outPos;
  outPos = meter->GetStartPointCoordinate();
  CPPUNIT_ASSERT(outPos[0] == pos[0] && outPos[1] == pos[1] && outPos[2] == pos[2]);

  meter->ReparentTo(NULL);
  vmeParametricSurfaceSTART->ReparentTo(NULL);
  vmeParametricSurfaceEND1->ReparentTo(NULL);

  albaDEL(meter);
  albaDEL(vmeParametricSurfaceSTART);
  albaDEL(vmeParametricSurfaceEND1);

  albaDEL(storage);

}