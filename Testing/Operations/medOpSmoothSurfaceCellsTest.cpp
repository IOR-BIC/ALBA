/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpSmoothSurfaceCellsTest.cpp,v $
Language:  C++
Date:      $Date: 2010-03-23 16:39:40 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "medOpSmoothSurfaceCellsTest.h"
#include "medOpSmoothSurfaceCells.h"

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
void medOpSmoothSurfaceCellsTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpSmoothSurfaceCells *op = new medOpSmoothSurfaceCells();
  mafDEL(op);
}
//-----------------------------------------------------------
void medOpSmoothSurfaceCellsTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  medOpSmoothSurfaceCells op; 
}
//-----------------------------------------------------------
void medOpSmoothSurfaceCellsTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMESurface> surface;

  medOpSmoothSurfaceCells *op = new medOpSmoothSurfaceCells();
  op->TestModeOn();

  CPPUNIT_ASSERT(!op->Accept(group));
  CPPUNIT_ASSERT(!op->Accept(volume));
  CPPUNIT_ASSERT(op->Accept(surface));

  mafDEL(op);

}
//-----------------------------------------------------------
void medOpSmoothSurfaceCellsTest::TestSetDiameter() 
//-----------------------------------------------------------
{
  medOpSmoothSurfaceCells *op = new medOpSmoothSurfaceCells();
  op->TestModeOn();
  
  op->SetDiameter(5.0);

  CPPUNIT_ASSERT( op->GetDiameter() == 5.0 );

  op->SetDiameter(0.2);

  CPPUNIT_ASSERT( op->GetDiameter() == 0.2 );

  mafDEL(op);

}
//-----------------------------------------------------------
void medOpSmoothSurfaceCellsTest::TestOpRun() 
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

  medOpSmoothSurfaceCells *op = new medOpSmoothSurfaceCells();
  op->TestModeOn();
  op->SetInput(input);
  op->OpRun();

  mafDEL(op);

}
//-----------------------------------------------------------
void medOpSmoothSurfaceCellsTest::TestSetSeed() 
//-----------------------------------------------------------
{
  medOpSmoothSurfaceCells *op = new medOpSmoothSurfaceCells();
  op->TestModeOn();
  op->SetSeed(3);
  
  CPPUNIT_ASSERT( op->GetSeed()==3 );
  
  op->SetSeed(10);

  CPPUNIT_ASSERT( op->GetSeed()==10 );


  mafDEL(op);

}
//-----------------------------------------------------------
void medOpSmoothSurfaceCellsTest::TestMarkCells() 
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

  medOpSmoothSurfaceCells *op = new medOpSmoothSurfaceCells();
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
void medOpSmoothSurfaceCellsTest::TestSmoothCells() 
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

  medOpSmoothSurfaceCells *op = new medOpSmoothSurfaceCells();
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
