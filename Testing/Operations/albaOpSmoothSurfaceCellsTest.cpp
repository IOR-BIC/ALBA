/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSmoothSurfaceCellsTest
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
#include "albaOpSmoothSurfaceCellsTest.h"
#include "albaOpSmoothSurfaceCells.h"

#include "albaVMEGroup.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"

#include "vtkALBASmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkCleanPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkALBARemoveCellsFilter.h"

#define TOLERANCE 0.001

//-----------------------------------------------------------
void albaOpSmoothSurfaceCellsTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpSmoothSurfaceCells *op = new albaOpSmoothSurfaceCells();
  albaDEL(op);
}
//-----------------------------------------------------------
void albaOpSmoothSurfaceCellsTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpSmoothSurfaceCells op; 
}
//-----------------------------------------------------------
void albaOpSmoothSurfaceCellsTest::TestAccept() 
//-----------------------------------------------------------
{
  albaSmartPointer<albaVMEGroup> group;
  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMESurface> surface;

  albaOpSmoothSurfaceCells *op = new albaOpSmoothSurfaceCells();
  op->TestModeOn();

  CPPUNIT_ASSERT(!op->Accept(group));
  CPPUNIT_ASSERT(!op->Accept(volume));
  CPPUNIT_ASSERT(op->Accept(surface));

  albaDEL(op);

}
//-----------------------------------------------------------
void albaOpSmoothSurfaceCellsTest::TestSetDiameter() 
//-----------------------------------------------------------
{
  albaOpSmoothSurfaceCells *op = new albaOpSmoothSurfaceCells();
  op->TestModeOn();
  
  op->SetDiameter(5.0);

  CPPUNIT_ASSERT( op->GetDiameter() == 5.0 );

  op->SetDiameter(0.2);

  CPPUNIT_ASSERT( op->GetDiameter() == 0.2 );

  albaDEL(op);

}
//-----------------------------------------------------------
void albaOpSmoothSurfaceCellsTest::TestOpRun() 
//-----------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  //Only a test to check leaks
  //////////////////////////////////////////////////////////////////////////

  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  albaSmartPointer<albaVMESurface> input;
  input->SetData(sphere->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaOpSmoothSurfaceCells *op = new albaOpSmoothSurfaceCells();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();

  albaDEL(op);

}
//-----------------------------------------------------------
void albaOpSmoothSurfaceCellsTest::TestSetSeed() 
//-----------------------------------------------------------
{
  albaOpSmoothSurfaceCells *op = new albaOpSmoothSurfaceCells();
  op->TestModeOn();
  op->SetSeed(3);
  
  CPPUNIT_ASSERT( op->GetSeed()==3 );
  
  op->SetSeed(10);

  CPPUNIT_ASSERT( op->GetSeed()==10 );


  albaDEL(op);

}
//-----------------------------------------------------------
void albaOpSmoothSurfaceCellsTest::TestMarkCells() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(5.0);
  sphere->Update();

  albaSmartPointer<albaVMESurface> input;
  input->SetData(sphere->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaOpSmoothSurfaceCells *op = new albaOpSmoothSurfaceCells();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->SetSeed(0);
  op->SetDiameter(0.5);//To select only one cell
  op->MarkCells();

  for (int i=0;i<sphere->GetOutput()->GetNumberOfCells();i++)
  {
    bool sel = op->CellIsSelected(i);
    if (i == 0)
    {
      CPPUNIT_ASSERT( sel == true );
    }
    else
    {
      CPPUNIT_ASSERT( sel == false );
    }
  }

  albaDEL(op);

}
//-----------------------------------------------------------
void albaOpSmoothSurfaceCellsTest::TestSmoothCells() 
//-----------------------------------------------------------
{
  vtkALBASmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(2.0);
  sphere->Update();

  int s = sphere->GetOutput()->GetNumberOfCells();

  albaSmartPointer<albaVMESurface> input;
  input->SetData(sphere->GetOutput(),0.0);
  input->GetOutput()->Update();
  input->Update();

  albaOpSmoothSurfaceCells *op = new albaOpSmoothSurfaceCells();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();
  op->SetSeed(0);
  op->SetDiameter(5.0);
  op->MarkCells();
  //Save the list of cell selected
  bool *cellSelecetd = new bool[sphere->GetOutput()->GetNumberOfCells()];
  for (int i=0;i<sphere->GetOutput()->GetNumberOfCells();i++)
  {
    cellSelecetd[i] = op->CellIsSelected(i);
  }
  
  op->SmoothCells();
  op->OpDo();

  vtkPolyData *output = vtkPolyData::SafeDownCast(albaVMESurface::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData());

  //////////////////////////////////////////////////////////////////////////
  //Extract the marked cell
  //////////////////////////////////////////////////////////////////////////
  vtkALBASmartPointer<vtkALBARemoveCellsFilter> cellFilter1;
  vtkALBASmartPointer<vtkALBARemoveCellsFilter> cellFilter2;
  cellFilter1->SetInputConnection(sphere->GetOutputPort());
  cellFilter1->Update();
  cellFilter2->SetInputConnection(sphere->GetOutputPort());
  cellFilter2->Update();
  for (int j=0;j<sphere->GetOutput()->GetNumberOfCells();j++)
  {
	  if (cellSelecetd[j] == true)
	  {
      cellFilter1->MarkCell(j);
      cellFilter2->MarkCell(j);
	  }
  }

  cellFilter1->ReverseRemoveOn();
  cellFilter1->RemoveMarkedCells();
  cellFilter1->Update();

  vtkALBASmartPointer<vtkPolyData> toSmoothPolyData;
  toSmoothPolyData->DeepCopy(cellFilter1->GetOutput());

  cellFilter2->ReverseRemoveOff();
  cellFilter2->RemoveMarkedCells();
  cellFilter2->Update();

  vtkALBASmartPointer<vtkPolyData> polyData;
  polyData->DeepCopy(cellFilter2->GetOutput());

  vtkALBASmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetInputData(toSmoothPolyData);
  smoothFilter->SetNumberOfIterations(op->GetNumberOfInteractions());
  smoothFilter->BoundarySmoothingOff();//always true
  smoothFilter->FeatureEdgeSmoothingOn();
  smoothFilter->SetFeatureAngle(op->GetFeatureAngle());
  smoothFilter->Update();

  vtkALBASmartPointer<vtkAppendPolyData> appendFilter; 
  appendFilter->AddInputConnection(smoothFilter->GetOutputPort());
  appendFilter->AddInputData(polyData);
  appendFilter->Update();

  vtkALBASmartPointer<vtkCleanPolyData> cleanFilter; 
  cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
  cleanFilter->Update();

  vtkALBASmartPointer<vtkPolyData> outputFilter;
  outputFilter->DeepCopy(cleanFilter->GetOutput());
  
  vtkDataArray *n1 = outputFilter->GetCellData()->GetNormals();
  vtkDataArray *n2 = output->GetCellData()->GetNormals();

  double pt1[3],pt2[3];
  for (int i=0;i<output->GetNumberOfPoints();i++)
  {
    bool found = false;
    output->GetPoint(i,pt1);
    for (int j=0;j<cleanFilter->GetOutput()->GetNumberOfPoints();j++)
    {
      cleanFilter->GetOutput()->GetPoint(j,pt2);
      if (abs(pt1[0] - pt2[0]) < TOLERANCE && abs(pt1[1] - pt2[1]) < TOLERANCE && abs(pt1[2] - pt2[2]) < TOLERANCE)
      {
        found = true;
        break;
      }
    }

    CPPUNIT_ASSERT( found );
  }

  delete []cellSelecetd;
  albaDEL(op);

}
