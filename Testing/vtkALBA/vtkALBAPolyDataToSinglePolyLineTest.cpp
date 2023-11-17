/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPolyDataToSinglePolyLineTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include <cppunit/config/SourcePrefix.h>

#include "vtkALBAPolyDataToSinglePolyLineTest.h"
#include "vtkALBAPolyDataToSinglePolyLine.h"
#include "vtkALBASmartPointer.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"


//--------------------------------------------------
void vtkALBAPolyDataToSinglePolyLineTest::TestFixture()
//--------------------------------------------------
{

}
//--------------------------------------------------
void vtkALBAPolyDataToSinglePolyLineTest::TestConversion()
//--------------------------------------------------
{
  vtkALBASmartPointer<vtkPolyData> polyOriginal;

  vtkALBASmartPointer<vtkPoints> points;
  vtkALBASmartPointer<vtkCellArray> cellArray;

  polyOriginal->SetPoints(points);
  polyOriginal->SetLines(cellArray);

  points->InsertNextPoint(0.0,0.0,0.0);  // first point - first cell
  points->InsertNextPoint(0.0,0.0,1.0);  // second point - first cell

  points->InsertNextPoint(0.0,1.0,0.0);  // first point - second cell
  points->InsertNextPoint(0.0,1.0,1.0);  // second point - second cell

  points->InsertNextPoint(1.0,0.0,0.0); // first point - third cell
  points->InsertNextPoint(1.0,0.0,1.0); // second point - third cell

  points->InsertNextPoint(1.0,1.0,0.0); // first point - third cell
  points->InsertNextPoint(1.0,1.0,1.0); // second point - third cell

  //create Lines
  vtkIdType pointId[2];
  for(int i = 0; i< points->GetNumberOfPoints();i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      cellArray->InsertNextCell(2 , pointId);  
    }
  }

  polyOriginal->Update();

  vtkALBASmartPointer<vtkALBAPolyDataToSinglePolyLine> psp;
  psp->SetInput(polyOriginal);
  psp->Update();

  vtkPolyData *polyResult = psp->GetOutput();

  CPPUNIT_ASSERT(polyResult->GetNumberOfCells() == 1);

  for(int i = 0; i<polyOriginal->GetNumberOfPoints(); i++)
  {
    double pO[3], pR[3];
    polyOriginal->GetPoint(i, pO);
    polyResult->GetPoint(i, pR);

    CPPUNIT_ASSERT(pO[0] == pR[0] &&
                   pO[1] == pR[1] &&
                   pO[2] == pR[2]);
  }
}