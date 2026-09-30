/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputWrappedMeterTest
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
#include "albaVMEOutputWrappedMeterTest.h"

#include "albaVMEOutputWrappedMeter.h"

#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaVMESurface.h"
#include "albaVMEWrappedMeter.h"

#include "vtkALBASmartPointer.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <iostream>


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)
#define TOLERANCE 1.0e-3

//----------------------------------------------------------------------------
void albaVMEOutputWrappedMeterTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputWrappedMeterTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputWrappedMeter outputWrappedMeter;
}
//----------------------------------------------------------------------------
void albaVMEOutputWrappedMeterTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputWrappedMeter *outputWrappedMeter = new albaVMEOutputWrappedMeter();
  cppDEL(outputWrappedMeter);
}
//----------------------------------------------------------------------------
void albaVMEOutputWrappedMeterTest::TestGetPolylineData_Update()
//----------------------------------------------------------------------------
{
  //create a wrapped meter
  albaVMEWrappedMeter *vmeWrappedMeter;
  albaNEW(vmeWrappedMeter);

  /*vtkALBASmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(1.0,1.0,1.0);

  vtkALBASmartPointer<vtkCellArray> cells;
  vtkIdType lineids[2];
  lineids[0] = 0;
  lineids[1] = 1;
  cells->InsertNextCell(2,lineids);

  vtkALBASmartPointer<vtkPolyData> polyline;
  polyline->SetPoints(pts);
  polyline->SetLines(cells);
  vmePolylineEditor->SetData(polyline, 0.0);*/

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
  vmeWrappedMeter->ReparentTo(storage->GetRoot());

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

  vmeWrappedMeter->SetMeterLink("StartVME", surface);
  vmeWrappedMeter->SetMeterLink("EndVME1", surface2);
  vmeWrappedMeter->SetMeterLink("WrappedVME", surface3);

  albaVMEOutputWrappedMeter *outputWrappedMeter = NULL;
  outputWrappedMeter = albaVMEOutputWrappedMeter::SafeDownCast(vmeWrappedMeter->GetOutput());
  outputWrappedMeter->Update();
  double distance = outputWrappedMeter->GetPolylineData()->GetLength();
  m_Result = outputWrappedMeter != NULL && 
    (vtkPolyData*)vmeWrappedMeter->GetOutput()->GetVTKData() == outputWrappedMeter->GetPolylineData() &&
    distance == vmeWrappedMeter->GetDistance();

  TEST_RESULT;

  vmeWrappedMeter->ReparentTo(NULL);

  surface->ReparentTo(NULL);
  surface2->ReparentTo(NULL);
  surface3->ReparentTo(NULL);

  albaDEL(surface);
  albaDEL(surface2);
  albaDEL(surface3);
  albaDEL(vmeWrappedMeter);
  albaDEL(storage);
}