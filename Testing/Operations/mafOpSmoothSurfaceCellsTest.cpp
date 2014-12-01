/*=========================================================================

 Program: MAF2Medical
 Module: mafOpSmoothSurfaceCellsTest
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
#include "mafOpSmoothSurfaceCellsTest.h"
#include "mafOpSmoothSurfaceCells.h"

#include "mafVMEGroup.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"
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
#include "vtkMAFRemoveCellsFilter.h"

#define TOLERANCE 0.001

//-----------------------------------------------------------
void mafOpSmoothSurfaceCellsTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpSmoothSurfaceCells *op = new mafOpSmoothSurfaceCells();
  mafDEL(op);
}
//-----------------------------------------------------------
void mafOpSmoothSurfaceCellsTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpSmoothSurfaceCells op; 
}
//-----------------------------------------------------------
void mafOpSmoothSurfaceCellsTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMESurface> surface;

  mafOpSmoothSurfaceCells *op = new mafOpSmoothSurfaceCells();
  op->TestModeOn();

  CPPUNIT_ASSERT(!op->Accept(group));
  CPPUNIT_ASSERT(!op->Accept(volume));
  CPPUNIT_ASSERT(op->Accept(surface));

  mafDEL(op);

}
//-----------------------------------------------------------
void mafOpSmoothSurfaceCellsTest::TestSetDiameter() 
//-----------------------------------------------------------
{
  mafOpSmoothSurfaceCells *op = new mafOpSmoothSurfaceCells();
  op->TestModeOn();
  
  op->SetDiameter(5.0);

  CPPUNIT_ASSERT( op->GetDiameter() == 5.0 );

  op->SetDiameter(0.2);

  CPPUNIT_ASSERT( op->GetDiameter() == 0.2 );

  mafDEL(op);

}
//-----------------------------------------------------------
void mafOpSmoothSurfaceCellsTest::TestOpRun() 
//-----------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  //Only a test to check leaks
  //////////////////////////////////////////////////////////////////////////

  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->Update();

  mafSmartPointer<mafVMESurface> input;
  input->SetData(sphere->GetOutput(),0.0);
  input->GetOutput()->GetVTKData()->Update();
  input->GetOutput()->Update();
  input->Update();

  mafOpSmoothSurfaceCells *op = new mafOpSmoothSurfaceCells();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();

  mafDEL(op);

}
//-----------------------------------------------------------
void mafOpSmoothSurfaceCellsTest::TestSetSeed() 
//-----------------------------------------------------------
{
  mafOpSmoothSurfaceCells *op = new mafOpSmoothSurfaceCells();
  op->TestModeOn();
  op->SetSeed(3);
  
  CPPUNIT_ASSERT( op->GetSeed()==3 );
  
  op->SetSeed(10);

  CPPUNIT_ASSERT( op->GetSeed()==10 );


  mafDEL(op);

}
//-----------------------------------------------------------
void mafOpSmoothSurfaceCellsTest::TestMarkCells() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(5.0);
  sphere->Update();

  mafSmartPointer<mafVMESurface> input;
  input->SetData(sphere->GetOutput(),0.0);
  input->GetOutput()->GetVTKData()->Update();
  input->GetOutput()->Update();
  input->Update();

  mafOpSmoothSurfaceCells *op = new mafOpSmoothSurfaceCells();
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

  mafDEL(op);

}
//-----------------------------------------------------------
void mafOpSmoothSurfaceCellsTest::TestSmoothCells() 
//-----------------------------------------------------------
{
  vtkMAFSmartPointer<vtkSphereSource> sphere;
  sphere->SetRadius(2.0);
  sphere->Update();

  int s = sphere->GetOutput()->GetNumberOfCells();

  mafSmartPointer<mafVMESurface> input;
  input->SetData(sphere->GetOutput(),0.0);
  input->GetOutput()->GetVTKData()->Update();
  input->GetOutput()->Update();
  input->Update();

  mafOpSmoothSurfaceCells *op = new mafOpSmoothSurfaceCells();
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

  vtkPolyData *output = vtkPolyData::SafeDownCast(mafVMESurface::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData());
  output->Update();

  //////////////////////////////////////////////////////////////////////////
  //Extract the marked cell
  //////////////////////////////////////////////////////////////////////////
  vtkMAFSmartPointer<vtkMAFRemoveCellsFilter> cellFilter1;
  vtkMAFSmartPointer<vtkMAFRemoveCellsFilter> cellFilter2;
  cellFilter1->SetInput(sphere->GetOutput());
  cellFilter1->Update();
  cellFilter2->SetInput(sphere->GetOutput());
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

  vtkMAFSmartPointer<vtkPolyData> toSmoothPolyData;
  toSmoothPolyData->DeepCopy(cellFilter1->GetOutput());
  toSmoothPolyData->Update();

  cellFilter2->ReverseRemoveOff();
  cellFilter2->RemoveMarkedCells();
  cellFilter2->Update();

  vtkMAFSmartPointer<vtkPolyData> polyData;
  polyData->DeepCopy(cellFilter2->GetOutput());
  polyData->Update();

  vtkMAFSmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetInput(toSmoothPolyData);
  smoothFilter->SetNumberOfIterations(op->GetNumberOfInteractions());
  smoothFilter->BoundarySmoothingOff();//always true
  smoothFilter->FeatureEdgeSmoothingOn();
  smoothFilter->SetFeatureAngle(op->GetFeatureAngle());
  smoothFilter->Update();

  vtkMAFSmartPointer<vtkAppendPolyData> appendFilter; 
  appendFilter->AddInput(smoothFilter->GetOutput());
  appendFilter->AddInput(polyData);
  appendFilter->Update();

  vtkMAFSmartPointer<vtkCleanPolyData> cleanFilter; 
  cleanFilter->SetInput(appendFilter->GetOutput());
  cleanFilter->Update();

  vtkMAFSmartPointer<vtkPolyData> outputFilter;
  outputFilter->DeepCopy(cleanFilter->GetOutput());
  outputFilter->Update();

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
  mafDEL(op);

}
