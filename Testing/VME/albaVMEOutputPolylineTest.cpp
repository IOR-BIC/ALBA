/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputPolylineTest
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
#include "albaVMEOutputPolylineTest.h"

#include "albaVMEOutputPolyline.h"
#include "albaVMEPolyline.h"
#include "mmaMaterial.h"

#include "vtkALBASmartPointer.h"
#include "vtkDataSet.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaVMEOutputPolylineTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputPolyline outputPolyline;
}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputPolyline *outputPolyline = new albaVMEOutputPolyline();
  cppDEL(outputPolyline);
}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineTest::TestUpdate()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(0.0,1.0,0.0);
  pts->InsertNextPoint(0.0,2.5,0.0);
  pts->InsertNextPoint(0.0,2.5,3.0);
  
  vtkIdType lineids[3][2] = {
    {0,1},
    {1,2},
    {3,4}
  };

  vtkALBASmartPointer<vtkCellArray> cells;
  cells->InsertNextCell(2,lineids[0]);
  cells->InsertNextCell(2,lineids[1]);
  cells->InsertNextCell(2,lineids[2]);

  vtkALBASmartPointer<vtkPolyData> polylineVTK;
  polylineVTK->SetPoints(pts);
  polylineVTK->SetLines(cells);


  albaVMEPolyline *polyline;
  albaNEW(polyline);
  polyline->SetData(polylineVTK,0.0);
  polyline->Modified();
  polyline->Update();


  albaVMEOutputPolyline *outputPolyline = albaVMEOutputPolyline::SafeDownCast(polyline->GetOutput());
  outputPolyline->Update();

  albaString length = outputPolyline->GetLengthSTR();
  m_Result = length == "5.50";
  TEST_RESULT;

  albaString numOfPoints = outputPolyline->GetNumberOfPointsSTR();
  m_Result = numOfPoints == "4";
  TEST_RESULT;

  albaDEL(polyline);

}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineTest::TestCalculateLength()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(0.0,0.0,0.0);
  pts->InsertNextPoint(0.0,1.0,0.0);
  pts->InsertNextPoint(0.0,2.5,0.0);
  pts->InsertNextPoint(0.0,2.5,3.0);

  vtkIdType lineids[3][2] = {
    {0,1},
    {1,2},
    {3,4}
  };

  vtkALBASmartPointer<vtkCellArray> cells;
  cells->InsertNextCell(2,lineids[0]);
  cells->InsertNextCell(2,lineids[1]);
  cells->InsertNextCell(2,lineids[2]);

  vtkALBASmartPointer<vtkPolyData> polylineVTK;
  polylineVTK->SetPoints(pts);
  polylineVTK->SetLines(cells);


  albaVMEPolyline *polyline;
  albaNEW(polyline);
  polyline->SetData(polylineVTK,0.0);
  polyline->Modified();
  polyline->Update();


  albaVMEOutputPolyline *outputPolyline = albaVMEOutputPolyline::SafeDownCast(polyline->GetOutput());
  outputPolyline->Update();

  double length = outputPolyline->CalculateLength();
  m_Result = length == 5.50;
  TEST_RESULT;

  albaDEL(polyline);

}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineTest::TestGetPolylineData()
//----------------------------------------------------------------------------
{
  double  pointsCoord[4][3] = {
    {0.0,0.0,0.0},
    {0.0,1.0,0.0},
    {0.0,2.5,0.0},
    {0.0,2.5,3.0}
  };

  vtkALBASmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(pointsCoord[0]);
  pts->InsertNextPoint(pointsCoord[1]);
  pts->InsertNextPoint(pointsCoord[2]);
  pts->InsertNextPoint(pointsCoord[3]);

  vtkIdType lineids[3][2] = {
    {0,1},
    {1,2},
    {3,4}
  };

  vtkALBASmartPointer<vtkCellArray> cells;
  cells->InsertNextCell(2,lineids[0]);
  cells->InsertNextCell(2,lineids[1]);
  cells->InsertNextCell(2,lineids[2]);

  vtkALBASmartPointer<vtkPolyData> polylineVTK;
  polylineVTK->SetPoints(pts);
  polylineVTK->SetLines(cells);


  albaVMEPolyline *polyline;
  albaNEW(polyline);
  polyline->SetData(polylineVTK,0.0);
  polyline->Modified();
  polyline->Update();

  albaVMEOutputPolyline *outputPolyline = albaVMEOutputPolyline::SafeDownCast(polyline->GetOutput());
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

  albaDEL(polyline);

}
//----------------------------------------------------------------------------
void albaVMEOutputPolylineTest::TestGetMaterial()
//----------------------------------------------------------------------------
{
  double  pointsCoord[4][3] = {
    {0.0,0.0,0.0},
    {0.0,1.0,0.0},
    {0.0,2.5,0.0},
    {0.0,2.5,3.0}
  };

  vtkALBASmartPointer<vtkPoints> pts;
  pts->InsertNextPoint(pointsCoord[0]);
  pts->InsertNextPoint(pointsCoord[1]);
  pts->InsertNextPoint(pointsCoord[2]);
  pts->InsertNextPoint(pointsCoord[3]);

  vtkIdType lineids[3][2] = {
    {0,1},
    {1,2},
    {3,4}
  };

  vtkALBASmartPointer<vtkCellArray> cells;
  cells->InsertNextCell(2,lineids[0]);
  cells->InsertNextCell(2,lineids[1]);
  cells->InsertNextCell(2,lineids[2]);

  vtkALBASmartPointer<vtkPolyData> polylineVTK;
  polylineVTK->SetPoints(pts);
  polylineVTK->SetLines(cells);


  albaVMEPolyline *polyline;
  albaNEW(polyline);
  polyline->SetData(polylineVTK,0.0);
  polyline->Modified();
  polyline->Update();

  albaVMEOutputPolyline *outputPolyline = albaVMEOutputPolyline::SafeDownCast(polyline->GetOutput());
  outputPolyline->Update();

  mmaMaterial *material = outputPolyline->GetMaterial();
  m_Result = material == NULL;
  TEST_RESULT;

  albaNEW(material);
  outputPolyline->SetMaterial(material);
    
  mmaMaterial *materialOutput = outputPolyline->GetMaterial();
  m_Result = material != NULL;
  TEST_RESULT;

  albaDEL(material);
  albaDEL(polyline);

}