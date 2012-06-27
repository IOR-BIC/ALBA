/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputPolylineTest
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
#include "mafVMEOutputPolylineTest.h"

#include "mafVMEOutputPolyline.h"
#include "mafVMEPolyline.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputPolyline outputPolyline;
}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputPolyline *outputPolyline = new mafVMEOutputPolyline();
  cppDEL(outputPolyline);
}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::TestUpdate()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(0.0,1.0,0.0);
  pts->InsertNextPoint(0.0,2.5,0.0);
  pts->InsertNextPoint(0.0,2.5,3.0);
  
  vtkIdType lineids[3][2] = {
    {0,1},
    {1,2},
    {3,4}
  };

  vtkMAFSmartPointer<vtkCellArray> cells;
  cells->InsertNextCell(2,lineids[0]);
  cells->InsertNextCell(2,lineids[1]);
  cells->InsertNextCell(2,lineids[2]);

  vtkMAFSmartPointer<vtkPolyData> polylineVTK;
  polylineVTK->SetPoints(pts);
  polylineVTK->SetLines(cells);
  polylineVTK->Update();


  mafVMEPolyline *polyline;
  mafNEW(polyline);
  polyline->SetData(polylineVTK,0.0);
  polyline->Modified();
  polyline->Update();

  polyline->GetOutput()->GetVTKData()->Update();

  mafVMEOutputPolyline *outputPolyline = mafVMEOutputPolyline::SafeDownCast(polyline->GetOutput());
  outputPolyline->Update();

  mafString length = outputPolyline->GetLengthSTR();
  m_Result = length == "5.50";
  TEST_RESULT;

  mafString numOfPoints = outputPolyline->GetNumberOfPointsSTR();
  m_Result = numOfPoints == "4";
  TEST_RESULT;

  mafDEL(polyline);

}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::TestCalculateLength()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(0.0,1.0,0.0);
  pts->InsertNextPoint(0.0,2.5,0.0);
  pts->InsertNextPoint(0.0,2.5,3.0);

  vtkIdType lineids[3][2] = {
    {0,1},
    {1,2},
    {3,4}
  };

  vtkMAFSmartPointer<vtkCellArray> cells;
  cells->InsertNextCell(2,lineids[0]);
  cells->InsertNextCell(2,lineids[1]);
  cells->InsertNextCell(2,lineids[2]);

  vtkMAFSmartPointer<vtkPolyData> polylineVTK;
  polylineVTK->SetPoints(pts);
  polylineVTK->SetLines(cells);
  polylineVTK->Update();


  mafVMEPolyline *polyline;
  mafNEW(polyline);
  polyline->SetData(polylineVTK,0.0);
  polyline->Modified();
  polyline->Update();

  polyline->GetOutput()->GetVTKData()->Update();

  mafVMEOutputPolyline *outputPolyline = mafVMEOutputPolyline::SafeDownCast(polyline->GetOutput());
  outputPolyline->Update();

  double length = outputPolyline->CalculateLength();
  m_Result = length == 5.50;
  TEST_RESULT;

  mafDEL(polyline);

}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::TestGetPolylineData()
//----------------------------------------------------------------------------
{
  double  pointsCoord[4][3] = {
    {0.0,0.0,0.0},
    {0.0,1.0,0.0},
    {0.0,2.5,0.0},
    {0.0,2.5,3.0}
  };

  vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(pointsCoord[0]);
  pts->InsertNextPoint(pointsCoord[1]);
  pts->InsertNextPoint(pointsCoord[2]);
  pts->InsertNextPoint(pointsCoord[3]);

  vtkIdType lineids[3][2] = {
    {0,1},
    {1,2},
    {3,4}
  };

  vtkMAFSmartPointer<vtkCellArray> cells;
  cells->InsertNextCell(2,lineids[0]);
  cells->InsertNextCell(2,lineids[1]);
  cells->InsertNextCell(2,lineids[2]);

  vtkMAFSmartPointer<vtkPolyData> polylineVTK;
  polylineVTK->SetPoints(pts);
  polylineVTK->SetLines(cells);
  polylineVTK->Update();


  mafVMEPolyline *polyline;
  mafNEW(polyline);
  polyline->SetData(polylineVTK,0.0);
  polyline->Modified();
  polyline->Update();

  polyline->GetOutput()->GetVTKData()->Update();

  mafVMEOutputPolyline *outputPolyline = mafVMEOutputPolyline::SafeDownCast(polyline->GetOutput());
  outputPolyline->Update();

  vtkPolyData *polylineOut = outputPolyline->GetPolylineData();
  m_Result = polylineOut->GetNumberOfPoints() == 4;
  TEST_RESULT;
  m_Result = polylineOut->GetNumberOfLines() == 3;
  TEST_RESULT;

  for (int i=0;i<polylineOut->GetNumberOfPoints();i++)
  {
    double pt[3];
    polylineOut->GetPoint(i,pt);
    m_Result = pt[0] == pointsCoord[i][0] && pt[1] == pointsCoord[i][1] && pt[2] == pointsCoord[i][2];
    TEST_RESULT;
  }

  mafDEL(polyline);

}
//----------------------------------------------------------------------------
void mafVMEOutputPolylineTest::TestGetMaterial()
//----------------------------------------------------------------------------
{
  double  pointsCoord[4][3] = {
    {0.0,0.0,0.0},
    {0.0,1.0,0.0},
    {0.0,2.5,0.0},
    {0.0,2.5,3.0}
  };

  vtkMAFSmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(pointsCoord[0]);
  pts->InsertNextPoint(pointsCoord[1]);
  pts->InsertNextPoint(pointsCoord[2]);
  pts->InsertNextPoint(pointsCoord[3]);

  vtkIdType lineids[3][2] = {
    {0,1},
    {1,2},
    {3,4}
  };

  vtkMAFSmartPointer<vtkCellArray> cells;
  cells->InsertNextCell(2,lineids[0]);
  cells->InsertNextCell(2,lineids[1]);
  cells->InsertNextCell(2,lineids[2]);

  vtkMAFSmartPointer<vtkPolyData> polylineVTK;
  polylineVTK->SetPoints(pts);
  polylineVTK->SetLines(cells);
  polylineVTK->Update();


  mafVMEPolyline *polyline;
  mafNEW(polyline);
  polyline->SetData(polylineVTK,0.0);
  polyline->Modified();
  polyline->Update();

  polyline->GetOutput()->GetVTKData()->Update();

  mafVMEOutputPolyline *outputPolyline = mafVMEOutputPolyline::SafeDownCast(polyline->GetOutput());
  outputPolyline->Update();

  mmaMaterial *material = outputPolyline->GetMaterial();
  m_Result = material == NULL;
  TEST_RESULT;

  mafNEW(material);
  outputPolyline->SetMaterial(material);
    
  mmaMaterial *materialOutput = outputPolyline->GetMaterial();
  m_Result = material != NULL;
  TEST_RESULT;

  mafDEL(material);
  mafDEL(polyline);

}