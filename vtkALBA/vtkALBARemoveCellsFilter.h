/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkALBARemoveCellsFilter.h
  Language:  C++
  Version:   $Id: vtkALBARemoveCellsFilter.h,v 1.1.2.1 2009-02-12 10:53:24 aqd0 Exp $

  Copyright (c) Goodwin Lawlor 2003-2004
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.
  
  Some modifications by Stefano Perticoni in order to make it work
  under ALBA

=========================================================================*/

// .NAME vtkALBARemoveCellsFilter- actually remove a cell from a vtkPolyData object
// .SECTION Description
// vtkALBARemoveCellsFilter removes cells one at a time. vtkCellPicker could be used to get the CellId of the cell to
// be removed. Then call the RemoveCell method with the CellId. The UndoRemoveCells method restores the 
// PolyData to it original state. Alternatively, you can mark cells with MarkCell and remove them all in one go
// with RemoveMarkedCells. This is much more efficient for large datasets since the filter only has to execute
// once.

// .SECTION Caveats
// Trying to remove the last remaining cell in a dataset will restore all the removed cells!
// Any filter between this filter and the vtkPolyDataMapper should not alter the cell order

// .SECTION Thanks
// Goodwin Lawlor, University College Dublin

#ifndef __vtkALBARemoveCellsFilter_h
#define __vtkALBARemoveCellsFilter_h

#include "albaConfigure.h"
#include "vtkALBACellsFilter.h"

class vtkLookupTable;

class ALBA_EXPORT vtkALBARemoveCellsFilter : public vtkALBACellsFilter
{
public:
  // Description:
  // Constructs with initial values of zero.
  static vtkALBARemoveCellsFilter *New();

  vtkTypeMacro(vtkALBARemoveCellsFilter, vtkALBACellsFilter);
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
 int GetNumberOfDeletedCells();
 int GetNumberOfRemovedCells(); 

 //Description
 //Set the m_ReverseRemove variable to 0 or 1 , if 1 reverse the removal
 void ReverseRemoveOn(){ReverseRemove = 1;}
 void ReverseRemoveOff(){ReverseRemove = 0;}

protected:

  vtkALBARemoveCellsFilter();
  ~vtkALBARemoveCellsFilter(){};

  int ReverseRemove; //< if On remove all the cells that are  that are not marked
  

private:
  vtkALBARemoveCellsFilter(const vtkALBARemoveCellsFilter&);  // Not implemented.
  void operator=(const vtkALBARemoveCellsFilter&);  // Not implemented.
};

#endif


