/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCellsFilter.h
  Language:  C++
  RCS:   $Id: vtkCellsFilter.h,v 1.3 2007-04-19 14:54:42 ior01 Exp $

  Copyright (c) Goodwin Lawlor 2004
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

  Some modifications by Stefano Perticoni in order to make it work
  under MAF (www.openmaf.org)

=========================================================================*/
// .NAME vtkCellsFilter- abstract superclass for removing/selecting cells.
// .SECTION Description
// vtkCellsFilter is an abstract superclass for classes the remove or select cells.r

// .SECTION Thanks
// Goodwin Lawlor, University College Dublin

#ifndef __vtkCellsFilter_h
#define __vtkCellsFilter_h

#include "vtkMAFConfigure.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkIdList.h"
#include "vtkCharArray.h"
#include "vtkPolyData.h"

class vtkLookupTable;

class VTK_vtkMAF_EXPORT vtkCellsFilter : public vtkPolyDataToPolyDataFilter
{

public:
  static vtkCellsFilter *New();
  vtkTypeRevisionMacro(vtkCellsFilter, vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Remove a cell from the input polydata.  cellid is a cell id from the input to the filter
  // A recursive trick remaps the new cellids (from the output) to the old cellids (at the input) 
  // so that the cellid returned by vtkCellPicker is the correct one to use.
 //void RemoveCell(vtkIdType cellid);
 
   // Description:
  // Mark a cell to be removed by RemoveMarkedCells later.  
 void MarkCell(vtkIdType cellid);
 
   // Description:
  // Unmark any cell that has been marked already
 void UnmarkCell(vtkIdType cellid);
 
   // Description:
  // If a cells is unmarked- mark it... if its marked- unmark it.
 void ToggleCell(vtkIdType cellid); 
 
 // Description:
 // Undo all the marks made on cells
 void UndoMarks(){this->InitializeScalars(); this->MarkedCellIdList->Reset(); this->Scalars->Modified();}

 //int GetNumberOfDeletedCells(){return this->GetInput()->GetNumberOfCells() - this->GetOutput()->GetNumberOfCells();}
 
  // Description:
  // Set/Get the marked color... default is red
  void SetMarkedColor(double r, double g, double b);
  void SetMarkedColor(double rgb[3]) {this->SetMarkedColor(rgb[0], rgb[1], rgb[2]);};
  vtkGetVector3Macro(MarkedColor, double);


  // Description:
  // Set/Get the umarked color... default is white. You could set this to the same color as the vtkProperty
  // and users won't know they are seeing scalars.
  void SetUnmarkedColor(double r, double g, double b);
  void SetUnmarkedColor(double rgb[3]) {this->SetUnmarkedColor(rgb[0], rgb[1], rgb[2]);};
  vtkGetVector3Macro(UnmarkedColor, double);

  // Description:
  // Set/Get the marked opacity... default is 1.0 By setting this to 0.0 you can temporally remove the cells
  // and then permanently remove them with RemoveMarkedCells
  void SetMarkedOpacity(double opacity);
  vtkGetMacro(MarkedOpacity, double);

	int GetNumberOfMarkedCells(){return MarkedCellIdList->GetNumberOfIds();};

	vtkIdType GetIdMarkedCell(int i){return MarkedCellIdList->GetId(i);};


protected:
  vtkCellsFilter();
  ~vtkCellsFilter();
  
  vtkIdList *CellIdList;
  vtkIdList *MarkedCellIdList;
  
  int IsInitialized;
  int IsScalarsInitialized;
  
  vtkCharArray *Scalars;
  vtkLookupTable *LUT;
 
  void Initialize();
  void InitializeScalars();
  
  double MarkedColor[3];
  double UnmarkedColor[3];
  double MarkedOpacity;
  
  void Execute();

private:
  vtkCellsFilter(const vtkCellsFilter&);  // Not implemented.
  void operator=(const vtkCellsFilter&);  // Not implemented.
};

#endif


