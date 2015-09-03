/*=========================================================================

 Program: MAF2
 Module: mafVMEPointSetTest
 Authors: Matteo Giacomoni
 
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
#include "mafVMEPointSetTest.h"

#include "mafVMEPointSet.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSphereSource.h"


#define TEST_RESULT CPPUNIT_ASSERT(result);
#define NUM_OF_POINTS 10

//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::BeforeTest()
//----------------------------------------------------------------------------
{
  mafNEW(m_PointSetVme);
}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::AfterTest()
//----------------------------------------------------------------------------
{
  mafDEL(m_PointSetVme);
}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEPointSet *vme = NULL;
  mafNEW(vme);

  CPPUNIT_ASSERT (vme != NULL);

  mafDEL(vme);
}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestSetData()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData>polydata;
  vtkMAFSmartPointer<vtkPoints>pts;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
  	pts->InsertNextPoint((double)i,(double)i,(double)i);
  }
  
  polydata->SetPoints(pts);
  polydata->Update();

  result = m_PointSetVme->SetData(polydata,0.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  vtkMAFSmartPointer<vtkCellArray> cells;
  for (int i=0;i<NUM_OF_POINTS-1;i++)
  {
    int cell[2] = {i,i+1};
    cells->InsertNextCell(2,cell);
  }

  polydata->SetLines(cells);
  polydata->Update();

  result = m_PointSetVme->SetData(polydata,0.0) == MAF_ERROR;

  CPPUNIT_ASSERT(result);

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  result = m_PointSetVme->SetData(sphere->GetOutput(),0.0) == MAF_ERROR;

  CPPUNIT_ASSERT(result);

  delete wxLog::SetActiveTarget(NULL);

}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestAppendPoint()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData>polydata1;
  vtkMAFSmartPointer<vtkPoints>pts1;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts1->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata1->SetPoints(pts1);
  polydata1->Update();

  result = m_PointSetVme->SetData(polydata1,0.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  vtkMAFSmartPointer<vtkPolyData>polydata2;
  vtkMAFSmartPointer<vtkPoints>pts2;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts2->InsertNextPoint(0.0,0.0,(double)i);
  }

  polydata2->SetPoints(pts2);
  polydata2->Update();

  result = m_PointSetVme->SetData(polydata2,1.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  double newPoint[3] = {11.0,11.0,11.0};
  m_PointSetVme->AppendPoint(newPoint[0],newPoint[1],newPoint[2],0.0);

  vtkPolyData *newPolydata1 = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());
  newPolydata1->Update();

  result = newPolydata1->GetNumberOfPoints() == NUM_OF_POINTS+1;

  CPPUNIT_ASSERT(result);

  double lastPoint[3];
  newPolydata1->GetPoint(NUM_OF_POINTS,lastPoint);

  result = lastPoint[0] == newPoint[0] && lastPoint[1] == newPoint[1] && lastPoint[2] == newPoint[2];

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);
  vtkPolyData *newPolydata2 = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());
  newPolydata2->Update();

  result = newPolydata2->GetNumberOfPoints() == NUM_OF_POINTS;

  CPPUNIT_ASSERT(result);

  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestRemovePoint()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData>polydata1;
  vtkMAFSmartPointer<vtkPoints>pts1;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts1->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata1->SetPoints(pts1);
  polydata1->Update();

  result = m_PointSetVme->SetData(polydata1,0.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  vtkMAFSmartPointer<vtkPolyData>polydata2;
  vtkMAFSmartPointer<vtkPoints>pts2;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts2->InsertNextPoint(0.0,0.0,(double)i);
  }

  polydata2->SetPoints(pts2);
  polydata2->Update();

  result = m_PointSetVme->SetData(polydata2,1.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  //Remove the first point
  m_PointSetVme->RemovePoint(0,0.0);

  vtkPolyData *newPolydata = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());
  newPolydata->Update();

  result = newPolydata->GetNumberOfPoints() == NUM_OF_POINTS-1;

  CPPUNIT_ASSERT(result);

  double firstPoint[3];
  newPolydata->GetPoint(0,firstPoint);

  result = firstPoint[0] == 1.0 && firstPoint[1] == 1.0 && firstPoint[2] == 1.0;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);
  vtkPolyData *newPolydata2 = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());
  newPolydata2->Update();

  result = newPolydata2->GetNumberOfPoints() == NUM_OF_POINTS;

  CPPUNIT_ASSERT(result);


  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestSetPoint()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData>polydata1;
  vtkMAFSmartPointer<vtkPoints>pts1;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts1->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata1->SetPoints(pts1);
  polydata1->Update();

  result = m_PointSetVme->SetData(polydata1,0.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  vtkMAFSmartPointer<vtkPolyData>polydata2;
  vtkMAFSmartPointer<vtkPoints>pts2;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts2->InsertNextPoint(0.0,0.0,(double)i);
  }

  polydata2->SetPoints(pts2);
  polydata2->Update();

  result = m_PointSetVme->SetData(polydata2,1.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetPoint(2,0.0,0.0,1.0,0.0);

  vtkPolyData *newPolydata = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());
  newPolydata->Update();

  result = newPolydata->GetNumberOfPoints() == NUM_OF_POINTS;

  CPPUNIT_ASSERT(result);

  double pointToCheck[3];
  newPolydata->GetPoint(2,pointToCheck);

  result = pointToCheck[0] == 0.0 && pointToCheck[1] == 0.0 && pointToCheck[2] == 1.0;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);
  vtkPolyData *newPolydata2 = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());
  newPolydata2->Update();

  newPolydata->GetPoint(2,pointToCheck);

  result = pointToCheck[0] == 0.0 && pointToCheck[1] == 0.0 && pointToCheck[2] == 2.0;

  CPPUNIT_ASSERT(result);


  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestGetPoint()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData>polydata1;
  vtkMAFSmartPointer<vtkPoints>pts1;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts1->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata1->SetPoints(pts1);
  polydata1->Update();

  result = m_PointSetVme->SetData(polydata1,0.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  vtkMAFSmartPointer<vtkPolyData>polydata2;
  vtkMAFSmartPointer<vtkPoints>pts2;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts2->InsertNextPoint(0.0,0.0,(double)i);
  }

  polydata2->SetPoints(pts2);
  polydata2->Update();

  result = m_PointSetVme->SetData(polydata2,1.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  double pointToCheck[3];
  m_PointSetVme->GetPoint(2,pointToCheck,0.0);

  result = pointToCheck[0] == 2.0 && pointToCheck[1] == 2.0 && pointToCheck[2] == 2.0;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);

  m_PointSetVme->GetPoint(2,pointToCheck,1.0);

  result = pointToCheck[0] == 0.0 && pointToCheck[1] == 0.0 && pointToCheck[2] == 2.0;

  CPPUNIT_ASSERT(result);

  delete wxLog::SetActiveTarget(NULL);

}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestGetNumberOfPoints()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData>polydata;
  vtkMAFSmartPointer<vtkPoints>pts;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata->SetPoints(pts);
  polydata->Update();

  result = m_PointSetVme->SetData(polydata,0.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  result = m_PointSetVme->GetNumberOfPoints(0.0) == NUM_OF_POINTS;

  CPPUNIT_ASSERT(result);

  delete wxLog::SetActiveTarget(NULL);

}
//----------------------------------------------------------------------------
void mafVMEPointSetTest::TestSetNumberOfPoints()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyData>polydata;
  vtkMAFSmartPointer<vtkPoints>pts;

  for (int i=0;i<NUM_OF_POINTS;i++)
  {
    pts->InsertNextPoint((double)i,(double)i,(double)i);
  }

  polydata->SetPoints(pts);
  polydata->Update();

  result = m_PointSetVme->SetData(polydata,0.0) == MAF_OK;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetNumberOfPoints(5,0.0);;

  vtkPolyData *newPolydata = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());
  newPolydata->Update();

  result = newPolydata->GetNumberOfPoints() == 5;

  CPPUNIT_ASSERT(result);

  m_PointSetVme->SetTimeStamp(1.0);
  m_PointSetVme->SetNumberOfPoints(1,1.0);

  newPolydata = vtkPolyData::SafeDownCast(m_PointSetVme->GetOutput()->GetVTKData());
  newPolydata->Update();

  result = newPolydata->GetNumberOfPoints() == 1;

  CPPUNIT_ASSERT(result);

  delete wxLog::SetActiveTarget(NULL);

}
