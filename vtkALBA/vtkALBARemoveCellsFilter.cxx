/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkALBARemoveCellsFilter.cxx
  Language:  C++
  Version:   $Id: vtkALBARemoveCellsFilter.cxx,v 1.1.2.1 2009-02-12 10:53:24 aqd0 Exp $

  Copyright (c) 2003-2004 Goodwin Lawlor
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE. 

  Some modifications by Stefano Perticoni in order to make it work
  under ALBA

=========================================================================*/
#include "vtkALBARemoveCellsFilter.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkALBARemoveCellsFilter);

vtkALBARemoveCellsFilter::vtkALBARemoveCellsFilter()
{
  ReverseRemove = 0;
}

void vtkALBARemoveCellsFilter::RemoveMarkedCells()
{
  
  vtkIdType numIds = this->MarkedCellIdList->GetNumberOfIds();
  
  if(ReverseRemove == 1)
  {
    this->CellIdList->DeepCopy(this->MarkedCellIdList);
  }
  else
  {
    for (vtkIdType i = 0; i < numIds; i++)
    {
      this->CellIdList->DeleteId(this->MarkedCellIdList->GetId(i));
    }
  }
  
  this->InitializeScalars();
  this->Modified();
}

//---------------------------------------------------------------------------
int vtkALBARemoveCellsFilter::GetNumberOfDeletedCells()
{
	vtkPolyData *input=vtkPolyData::SafeDownCast(this->GetInput());
	vtkPolyData *output=vtkPolyData::SafeDownCast(this->GetOutput());

	return input->GetNumberOfCells() - output->GetNumberOfCells();
}

//---------------------------------------------------------------------------
int vtkALBARemoveCellsFilter::GetNumberOfRemovedCells()
{
	vtkPolyData *input=vtkPolyData::SafeDownCast(this->GetInput());
	vtkPolyData *output=vtkPolyData::SafeDownCast(this->GetOutput());

	return input->GetNumberOfCells() - output->GetNumberOfCells();
}

void vtkALBARemoveCellsFilter::RemoveCell(vtkIdType cellid_at_output)
{
  vtkIdType cellid_at_input;
  
  if (!this->IsInitialized)
    {
    this->Initialize();
    }
  
  // Because vtkIdList reorders its id list in the same manner that vtkCellArray
  // reorders its cell list, CellIdList can be used to find out what cellid a cell has
  // at the filter input... as cells are deleted from the middle of the cell array, 
  // ids are reorder so they're monotonic increasing (1,2,3,4,...)
  cellid_at_input = this->CellIdList->GetId(cellid_at_output);
  
  this->CellIdList->DeleteId(cellid_at_input); 
  this->Modified();
}

void vtkALBARemoveCellsFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Number of cells removed: "
     << this->GetNumberOfRemovedCells()
     << "\n";

}
