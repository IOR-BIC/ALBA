/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMAFRemoveCellsFilter.h
  Language:  C++
  Version:   $Id: vtkMAFRemoveCellsFilter.h,v 1.1.2.1 2009-02-12 10:53:24 aqd0 Exp $

  Copyright (c) Goodwin Lawlor 2003-2004
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.
  
  Some modifications by Stefano Perticoni in order to make it work
  under MAF (www.openmaf.org)

=========================================================================*/

// .NAME vtkMAFRemoveCellsFilter- actually remove a cell from a vtkPolyData object
// .SECTION Description
// vtkMAFRemoveCellsFilter removes cells one at a time. vtkCellPicker could be used to get the CellId of the cell to
// be removed. Then call the RemoveCell method with the CellId. The UndoRemoveCells method restores the 
// PolyData to it original state. Alternatively, you can mark cells with MarkCell and remove them all in one go
// with RemoveMarkedCells. This is much more efficient for large datasets since the filter only has to execute
// once.

// .SECTION Caveats
// Trying to remove the last remaining cell in a dataset will restore all the removed cells!
// Any filter between this filter and the vtkPolyDataMapper should not alter the cell order

// .SECTION Thanks
// Goodwin Lawlor, University College Dublin

#ifndef __vtkMAFRemoveCellsFilter_h
#define __vtkMAFRemoveCellsFilter_h

#include "vtkMAFConfigure.h"
#include "vtkMAFCellsFilter.h"

class vtkLookupTable;

class VTK_vtkMAF_EXPORT vtkMAFRemoveCellsFilter : public vtkMAFCellsFilter
{
public:
  // Description:
  // Constructs with initial values of zero.
  static vtkMAFRemoveCellsFilter *New();

  vtkTypeRevisionMacro(vtkMAFRemoveCellsFilter, vtkMAFCellsFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Remove a cell from the input polydata.  cellid is a cell id from the input to the filter
  // A recursive trick remaps the new cellids (from the output) to the old cellids (at the input) 
  // so that the cellid returned by vtkCellPicker is the correct one to use.
 void RemoveCell(vtkIdType cellid);
 
  // Description:
  // Remove the cells marked by MarkCell(cellid)
 void RemoveMarkedCells();
 
 // Description:
 // Undo all the remove operations 
 void UndoRemoveCells(){this->Initialize(); this->Modified();}
 
 // Desciption:
 // Get the number of cells that have been removed. GetNumberOfDeletedCells should be depreciated.
 int GetNumberOfDeletedCells(){return this->GetInput()->GetNumberOfCells() - this->GetOutput()->GetNumberOfCells();}
 int GetNumberOfRemovedCells(){return this->GetInput()->GetNumberOfCells() - this->GetOutput()->GetNumberOfCells();} 

 //Description
 //Set the m_ReverseRemove variable to 0 or 1 , if 1 reverse the removal
 void ReverseRemoveOn(){ReverseRemove = 1;}
 void ReverseRemoveOff(){ReverseRemove = 0;}

protected:

  vtkMAFRemoveCellsFilter();
  ~vtkMAFRemoveCellsFilter(){};

  int ReverseRemove; //< if On remove all the cells that are  that are not marked
  

private:
  vtkMAFRemoveCellsFilter(const vtkMAFRemoveCellsFilter&);  // Not implemented.
  void operator=(const vtkMAFRemoveCellsFilter&);  // Not implemented.
};

#endif


