/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMAFCellsFilter.cxx
  Language:  C++
  Version:   $Id: vtkMAFCellsFilter.cxx,v 1.1.2.1 2009-01-08 11:42:44 ior01 Exp $

  Copyright (c) 2004 Goodwin Lawlor
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE. 

  Some modifications by Stefano Perticoni in order to make it work
  under MAF (www.openmaf.org)

=========================================================================*/
#include "vtkObjectFactory.h"

#include "vtkMAFCellsFilter.h"
#include "vtkIdList.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkCharArray.h"
#include "vtkLookupTable.h"

vtkCxxRevisionMacro(vtkMAFCellsFilter, "$Revision: 1.1.2.1 $");  
vtkStandardNewMacro(vtkMAFCellsFilter);

// Constructs with initial  values.
vtkMAFCellsFilter::vtkMAFCellsFilter()
{
  this->CellIdList= vtkIdList::New();
  this->MarkedCellIdList = vtkIdList::New();
  this->IsInitialized = 0;
  this->IsScalarsInitialized = 0;
  this->Scalars = vtkCharArray::New();
  this->Scalars->SetNumberOfComponents(1);
  this->Scalars->SetName("MarkedCells");
  this->MarkedColor[0] = 1.0;
  this->MarkedColor[0] = 0.0;
  this->MarkedColor[0] = 0.0;
  this->UnmarkedColor[0] = 1.0;
  this->UnmarkedColor[0] = 1.0;
  this->UnmarkedColor[0] = 1.0;
  this->MarkedOpacity = 1.0;
  this->LUT = vtkLookupTable::New();
  this->LUT->SetNumberOfColors(2);
  this->LUT->SetTableValue(0, 1.0, 1.0, 1.0, 1.0);
  this->LUT->SetTableValue(1, 1.0, 0.0, 0.0, 1.0);
  this->LUT->Build();
  this->Scalars->SetLookupTable(this->LUT);
}

// Destroy any allocated memory.
vtkMAFCellsFilter::~vtkMAFCellsFilter()
{
  this->CellIdList->Delete();
  this->MarkedCellIdList->Delete();
  this->Scalars->Delete();
  this->LUT->Delete();
}

void vtkMAFCellsFilter::SetMarkedColor(double r, double g, double b)
{
  this->LUT->SetTableValue(1, r, g, b, this->MarkedOpacity);
  this->MarkedColor[0] = r;
  this->MarkedColor[0] = g;
  this->MarkedColor[0] = b;
  this->LUT->Build();
  
}

void vtkMAFCellsFilter::SetUnmarkedColor(double r, double g, double b)
{
  this->LUT->SetTableValue(0, r, g, b, 1.0);
  this->UnmarkedColor[0] = r;
  this->UnmarkedColor[0] = g;
  this->UnmarkedColor[0] = b;
  this->LUT->Build();
  
}

void vtkMAFCellsFilter::SetMarkedOpacity(double opacity)
{
  double rgba[4];
  this->LUT->GetTableValue(1, rgba);
  this->LUT->SetTableValue(1, rgba[0], rgba[1], rgba[2], opacity);
  this->MarkedOpacity = opacity;
  this->LUT->Build();
}


// Description:
// Perform cell removal
void vtkMAFCellsFilter::Execute()
{
  if (!this->IsInitialized)
    {
    this->Initialize();
    }
    
  vtkPolyData *input = this->GetInput();
  vtkPolyData *output = this->GetOutput();
  
  // num of cells that will be in the output
  vtkIdType numCells = this->CellIdList->GetNumberOfIds();
  // dont know how many points... cant be more than the input
  vtkIdType numPts = input->GetNumberOfPoints();
  
  vtkPointData *outPD = output->GetPointData();
    outPD->CopyAllocate(input->GetPointData(), numPts);
  vtkCellData *outCD = output->GetCellData();
    outCD->CopyAllocate(input->GetCellData(),numCells);    
    output->Allocate(input,numCells);
    
    // Copy unremoved cells to the output... 
    output->CopyCells(input, this->CellIdList);
  
}

void vtkMAFCellsFilter::Initialize()
{
  // This updates the pipeline upstream so that we can get the number
  // of cells... horrific vtk filter design, but what else will work? I could
  // document to explicitly call ->Update() on previous filter. It wont
  // matter if the user lets the vtk pipeline execute once first and heads
  // into the event loop like the filter was intended to do originally.
  
  // This also gets around the problem when the user is in the event
  // loop but some filter has been modified upstream, changing the data.
  this->GetInput()->Update();
  
  vtkIdType numCells = this->GetInput()->GetNumberOfCells();
  this->CellIdList->SetNumberOfIds(numCells);
  
  for (vtkIdType i=0; i < numCells; i++)
    {
    this->CellIdList->SetId(i,i);
    }
    
  this->IsInitialized = 1;
}

void vtkMAFCellsFilter::InitializeScalars()
{
  // CellIdList will have been initialized first
  vtkIdType numCells = this->CellIdList->GetNumberOfIds();
  this->Scalars->SetNumberOfValues(numCells);
  

  
  for (vtkIdType i=0; i < numCells; i++)
    {
    this->Scalars->SetValue(i,0);
    }
  
  this->MarkedCellIdList->Reset();
  this->GetOutput()->GetCellData()->SetScalars(this->Scalars);
  this->IsScalarsInitialized = 1;
}

void vtkMAFCellsFilter::ToggleCell(vtkIdType cellid_at_output)
{
  vtkIdType cellid_at_input;
  
  // If the input to the filter has changed since the last execution
  // we need
  
  if (!this->IsInitialized)
    {
    this->Initialize();
    }
    
  // only initialize scalars if MarkCell or ToggleCells is used
  if (!this->IsScalarsInitialized)
    {
    this->InitializeScalars();
    }

  cellid_at_input = this->CellIdList->GetId(cellid_at_output);
  

  
  if (this->MarkedCellIdList->IsId(cellid_at_input) == -1)
    {
    this->MarkedCellIdList->InsertNextId(cellid_at_input); 
    this->Scalars->SetValue(cellid_at_output, 1);
    }
  else
    {
    this->MarkedCellIdList->DeleteId(cellid_at_input); 
    this->Scalars->SetValue(cellid_at_output, 0);
    }
    
  //do this here to avoid re-executing this filter...
  this->GetOutput()->GetCellData()->SetScalars(this->Scalars);
  this->Scalars->Modified();
    
}

void vtkMAFCellsFilter::UnmarkCell(vtkIdType cellid_at_output)
{
  vtkIdType cellid_at_input;
  
  // check if anything has been marked first  
  if (!this->IsScalarsInitialized)
    {
    return;
    }

  // see comment below
  cellid_at_input = this->CellIdList->GetId(cellid_at_output);
  
  
  this->MarkedCellIdList->DeleteId(cellid_at_input); 
  this->Scalars->SetValue(cellid_at_output, 0);
  
  //do this here to avoid re-executing this filter...
  this->Scalars->Modified();
    
}

void vtkMAFCellsFilter::MarkCell(vtkIdType cellid_at_output)
{
  vtkIdType cellid_at_input;

  // see comment below
  cellid_at_input = this->CellIdList->GetId(cellid_at_output);

  //check if the cell is already present in marked cells list
  int size = this->MarkedCellIdList->GetNumberOfIds();
  for(int i = 0; i< size; i++)
  {
    if(this->MarkedCellIdList->GetId(i) == cellid_at_input)
    {
      return; // cell already present in the list
    }
  }
   
  if (!this->IsInitialized)
    {
    this->Initialize();
    }
    
  // only initialize scalars if MarkCell is used
  if (!this->IsScalarsInitialized)
    {
    this->InitializeScalars();
    }

  

  this->MarkedCellIdList->InsertNextId(cellid_at_input); 
  this->Scalars->SetValue(cellid_at_output, 1);
  
  //do this here to avoid re-executing this filter...
  this->GetOutput()->GetCellData()->SetScalars(this->Scalars);
  this->Scalars->Modified();
    
}

void vtkMAFCellsFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
