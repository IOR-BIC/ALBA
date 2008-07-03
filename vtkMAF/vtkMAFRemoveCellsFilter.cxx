/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMAFRemoveCellsFilter.cxx
  Language:  C++
  Version:   $Id: vtkMAFRemoveCellsFilter.cxx,v 1.1 2008-07-03 11:27:45 aqd0 Exp $

  Copyright (c) 2003-2004 Goodwin Lawlor
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE. 

  Some modifications by Stefano Perticoni in order to make it work
  under MAF (www.openmaf.org)

=========================================================================*/
#include "vtkMAFRemoveCellsFilter.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFRemoveCellsFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFRemoveCellsFilter);

vtkMAFRemoveCellsFilter::vtkMAFRemoveCellsFilter()
{
  m_ReverseRemove = 0;
}

void vtkMAFRemoveCellsFilter::RemoveMarkedCells()
{
  
  vtkIdType numIds = this->MarkedCellIdList->GetNumberOfIds();
  
  if(m_ReverseRemove == 1)
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

void vtkMAFRemoveCellsFilter::RemoveCell(vtkIdType cellid_at_output)
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

void vtkMAFRemoveCellsFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Number of cells removed: "
     << this->GetNumberOfRemovedCells()
     << "\n";

}
