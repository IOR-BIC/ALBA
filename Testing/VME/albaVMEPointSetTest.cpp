/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPointSetTest
 Authors: Matteo Giacomoni
 
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
#include "albaVMEPointSetTest.h"

#include "albaVMEPointSet.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSphereSource.h"


#define TEST_RESULT CPPUNIT_ASSERT(result);
#define NUM_OF_POINTS 10

//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::BeforeTest()
//----------------------------------------------------------------------------
{
  albaNEW(m_PointSetVme);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::AfterTest()
//----------------------------------------------------------------------------
{
  albaDEL(m_PointSetVme);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEPointSet *vme = NULL;
  albaNEW(vme);

  CPPUNIT_ASSERT (vme != NULL);

  albaDEL(vme);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestSetData()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData>polydata;
  vtkALBASmartPointer<vtkPoints>pts;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
  	pts->InsertNextPoint((double)i,(double)i,(double)i);
  }
  
  polydata->SetPoints(pts);

  result = m_PointSetVme->SetData(polydata,0.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  vtkALBASmartPointer<vtkCellArray> cells;
  for (int i=0;i<NUM_OF_POINTS-1;i++)
  {
    vtkIdType cell[2] = {i,i+1};
    cells->InsertNextCell(2,cell);
  }

  polydata->SetLines(cells);

  result = m_PointSetVme->SetData(polydata,0.0) == ALBA_ERROR;

  CPPUNIT_ASSERT(result);

  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  result = m_PointSetVme->SetData(sphere->GetOutput(),0.0) == ALBA_ERROR;

  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestAppendPoint()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData>polydata1;
  vtkALBASmartPointer<vtkPoints>pts1;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts1->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata1->SetPoints(pts1);

  result = m_PointSetVme->SetData(polydata1,0.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  vtkALBASmartPointer<vtkPolyData>polydata2;
  vtkALBASmartPointer<vtkPoints>pts2;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts2->InsertNextPoint(0.0,0.0,(double)i);
  }

  polydata2->SetPoints(pts2);

  result = m_PointSetVme->SetData(polydata2,1.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  double newPoint[3] = {11.0,11.0,11.0};
  m_PointSetVme->AppendPoint(newPoint[0],newPoint[1],newPoint[2],0.0);

  vtkPolyData *newPolydata1 = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());

  result = newPolydata1->GetNumberOfPoints() == NUM_OF_POINTS+1;

  CPPUNIT_ASSERT(result);

  double lastPoint[3];
  newPolydata1->GetPoint(NUM_OF_POINTS,lastPoint);

  result = lastPoint[0] == newPoint[0] && lastPoint[1] == newPoint[1] && lastPoint[2] == newPoint[2];

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);
  vtkPolyData *newPolydata2 = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());

  result = newPolydata2->GetNumberOfPoints() == NUM_OF_POINTS;

  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestRemovePoint()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData>polydata1;
  vtkALBASmartPointer<vtkPoints>pts1;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts1->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata1->SetPoints(pts1);

  result = m_PointSetVme->SetData(polydata1,0.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  vtkALBASmartPointer<vtkPolyData>polydata2;
  vtkALBASmartPointer<vtkPoints>pts2;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts2->InsertNextPoint(0.0,0.0,(double)i);
  }

  polydata2->SetPoints(pts2);

  result = m_PointSetVme->SetData(polydata2,1.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  //Remove the first point
  m_PointSetVme->RemovePoint(0,0.0);

  vtkPolyData *newPolydata = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());

  result = newPolydata->GetNumberOfPoints() == NUM_OF_POINTS-1;

  CPPUNIT_ASSERT(result);

  double firstPoint[3];
  newPolydata->GetPoint(0,firstPoint);

  result = firstPoint[0] == 1.0 && firstPoint[1] == 1.0 && firstPoint[2] == 1.0;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);
  vtkPolyData *newPolydata2 = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());

  result = newPolydata2->GetNumberOfPoints() == NUM_OF_POINTS;

  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestSetPoint()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData>polydata1;
  vtkALBASmartPointer<vtkPoints>pts1;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts1->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata1->SetPoints(pts1);

  result = m_PointSetVme->SetData(polydata1,0.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  vtkALBASmartPointer<vtkPolyData>polydata2;
  vtkALBASmartPointer<vtkPoints>pts2;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts2->InsertNextPoint(0.0,0.0,(double)i);
  }

  polydata2->SetPoints(pts2);

  result = m_PointSetVme->SetData(polydata2,1.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetPoint(2,0.0,0.0,1.0,0.0);

  vtkPolyData *newPolydata = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());

  result = newPolydata->GetNumberOfPoints() == NUM_OF_POINTS;

  CPPUNIT_ASSERT(result);

  double pointToCheck[3];
  newPolydata->GetPoint(2,pointToCheck);

  result = pointToCheck[0] == 0.0 && pointToCheck[1] == 0.0 && pointToCheck[2] == 1.0;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);
  vtkPolyData *newPolydata2 = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());

  newPolydata->GetPoint(2,pointToCheck);

  result = pointToCheck[0] == 0.0 && pointToCheck[1] == 0.0 && pointToCheck[2] == 2.0;

  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestGetPoint()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData>polydata1;
  vtkALBASmartPointer<vtkPoints>pts1;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts1->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata1->SetPoints(pts1);

  result = m_PointSetVme->SetData(polydata1,0.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  vtkALBASmartPointer<vtkPolyData>polydata2;
  vtkALBASmartPointer<vtkPoints>pts2;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts2->InsertNextPoint(0.0,0.0,(double)i);
  }

  polydata2->SetPoints(pts2);

  result = m_PointSetVme->SetData(polydata2,1.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  double pointToCheck[3];
  m_PointSetVme->GetPoint(2,pointToCheck,0.0);

  result = pointToCheck[0] == 2.0 && pointToCheck[1] == 2.0 && pointToCheck[2] == 2.0;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);

  m_PointSetVme->GetPoint(2,pointToCheck,1.0);

  result = pointToCheck[0] == 0.0 && pointToCheck[1] == 0.0 && pointToCheck[2] == 2.0;

  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestGetNumberOfPoints()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData>polydata;
  vtkALBASmartPointer<vtkPoints>pts;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata->SetPoints(pts);

  result = m_PointSetVme->SetData(polydata,0.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  result = m_PointSetVme->GetNumberOfPoints(0.0) == NUM_OF_POINTS;

  CPPUNIT_ASSERT(result);
}
//----------------------------------------------------------------------------
void albaVMEPointSetTest::TestSetNumberOfPoints()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData>polydata;
  vtkALBASmartPointer<vtkPoints>pts;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata->SetPoints(pts);

  result = m_PointSetVme->SetData(polydata,0.0) == ALBA_OK;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetNumberOfPoints(5,0.0);;

  vtkPolyData *newPolydata = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());

  result = newPolydata->GetNumberOfPoints() == 5;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);
  m_PointSetVme->SetNumberOfPoints(1,1.0);

  newPolydata = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());

  result = newPolydata->GetNumberOfPoints() == 1;

  CPPUNIT_ASSERT(result);
}
