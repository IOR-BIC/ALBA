/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEOutputWrappedMeterTest.cpp,v $
Language:  C++
Date:      $Date: 2010-03-16 11:01:13 $
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
#include "medVMEOutputWrappedMeterTest.h"

#include "medVMEOutputWrappedMeter.h"

#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mafVMESurface.h"
#include "medVMEWrappedMeter.h"

#include "vtkMAFSmartPointer.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <iostream>


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void medVMEOutputWrappedMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medVMEOutputWrappedMeterTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void medVMEOutputWrappedMeterTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medVMEOutputWrappedMeterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  medVMEOutputWrappedMeter outputWrappedMeter;
}
//----------------------------------------------------------------------------
void medVMEOutputWrappedMeterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medVMEOutputWrappedMeter *outputWrappedMeter = new medVMEOutputWrappedMeter();
  cppDEL(outputWrappedMeter);
}
//----------------------------------------------------------------------------
void medVMEOutputWrappedMeterTest::TestGetPolylineData_Update()
//----------------------------------------------------------------------------
{
  //create a wrapped meter
  medVMEWrappedMeter *vmeWrappedMeter;
  mafNEW(vmeWrappedMeter);

  /*vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,1.0,1.0);

  vtkMAFSmartPointer<vtkCellArray> cells;
  vtkIdType lineids[2];
  lineids[0] = 0;
  lineids[1] = 1;
  cells->InsertNextCell(2,lineids);

  vtkMAFSmartPointer<vtkPolyData> polyline;
  polyline->SetPoints(pts);
  polyline->SetLines(cells);
  vmePolylineEditor->SetData(polyline, 0.0);*/

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
  vmeWrappedMeter->SetParent(storage->GetRoot());

  double pos[3];
  double rot[3];
  pos[0] = 0.0;
  pos[1] = 10.0;
  pos[2] = 0.0;
  rot[0] = rot[1] = rot[2] = 0.0;
  surface->SetPose(pos, rot, 0.0);

  double pos2[3];
  double rot2[3];
  pos2[0] = 0.0;
  pos2[1] = 0.0;
  pos2[2] = 0.0;
  rot2[0] = rot2[1] = rot2[2] = 0.0;
  surface2->SetPose(pos2, rot2, 0.0);

  double pos3[3];
  double rot3[3];
  pos3[0] = 0.0;
  pos3[1] = 5.0;
  pos3[2] = 0.0;
  rot3[0] = rot3[1] = rot3[2] = 0.0;
  surface3->SetPose(pos3, rot3, 0.0);

  vmeWrappedMeter->SetMeterLink("StartVME", mafNode::SafeDownCast(surface));
  vmeWrappedMeter->SetMeterLink("EndVME1", mafNode::SafeDownCast(surface2));
  vmeWrappedMeter->SetMeterLink("WrappedVME", mafNode::SafeDownCast(surface3));

  vmeWrappedMeter->GetOutput()->GetVTKData()->Update();

  medVMEOutputWrappedMeter *outputWrappedMeter = NULL;
  outputWrappedMeter = medVMEOutputWrappedMeter::SafeDownCast(vmeWrappedMeter->GetOutput());
  outputWrappedMeter->Update();
  double distance = outputWrappedMeter->GetPolylineData()->GetLength();
  m_Result = outputWrappedMeter != NULL && 
    (vtkPolyData*)vmeWrappedMeter->GetOutput()->GetVTKData() == outputWrappedMeter->GetPolylineData() &&
    distance == vmeWrappedMeter->GetDistance();

  TEST_RESULT;

  vmeWrappedMeter->SetParent(NULL);

  surface->SetParent(NULL);
  surface2->SetParent(NULL);
  surface3->SetParent(NULL);

  mafDEL(surface);
  mafDEL(surface2);
  mafDEL(surface3);
  mafDEL(vmeWrappedMeter);
  mafDEL(storage);
}