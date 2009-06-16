/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMAFCellsFilter.h
  Language:  C++
  RCS:   $Id: vtkMAFCellsFilter.h,v 1.1.2.1 2009-06-16 09:12:31 aqd0 Exp $

  Copyright (c) Goodwin Lawlor 2004
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

  Some modifications by Stefano Perticoni in order to make it work
  under MAF (www.openmaf.org)

=========================================================================*/

#ifndef __vtkMAFCellsFilter_h
#define __vtkMAFCellsFilter_h

#include "vtkMAFConfigure.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkIdList.h"
#include "vtkCharArray.h"
#include "vtkPolyData.h"

class vtkLookupTable;

/** 
 vtkMAFCellsFilter- superclass for selecting cells.
 
 vtkMAFCellsFilter is a superclass for classes to perform operations on selected cells.

 @sa
 vtkMAFRemoveCellsFilter
*/ 


class VTK_vtkMAF_EXPORT vtkMAFCellsFilter : public vtkPolyDataToPolyDataFilter
{

public:
  static vtkMAFCellsFilter *New();
  vtkTypeRevisionMacro(vtkMAFCellsFilter, vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
   
 /** Mark a cell */ 
 void MarkCell(vtkIdType cellid);
 
 /** Unmark any cell that has been marked already */
 void UnmarkCell(vtkIdType cellid);
 
 /** If a cells is unmarked- mark it... if its marked- unmark it. */
 void ToggleCell(vtkIdType cellid); 
 
 /** Undo all the marks made on cells */
 void UndoMarks(){this->InitializeScalars(); this->MarkedCellIdList->Reset(); this->Scalars->Modified();}

  /** Set/Get the marked color... default is red */
  void SetMarkedColor(double r, double g, double b);
  void SetMarkedColor(double rgb[3]) {this->SetMarkedColor(rgb[0], rgb[1], rgb[2]);};
  vtkGetVector3Macro(MarkedColor, double);

  /** Set/Get the umarked color... default is white. You could set this to the same color as the vtkProperty
  and users won't know they are seeing scalars */
  void SetUnmarkedColor(double r, double g, double b);
  void SetUnmarkedColor(double rgb[3]) {this->SetUnmarkedColor(rgb[0], rgb[1], rgb[2]);};
  vtkGetVector3Macro(UnmarkedColor, double);

  /** Set/Get the marked opacity... default is 1.0 By setting this to 0.0 you can temporally remove the cells
  and then permanently remove them with RemoveMarkedCells */
  void SetMarkedOpacity(double opacity);
  vtkGetMacro(MarkedOpacity, double);
  
  /** Get the number of marked cells */
	int GetNumberOfMarkedCells(){return MarkedCellIdList->GetNumberOfIds();};

  /** Get the marked cell id from the MarkedCellIdList */
	vtkIdType GetIdMarkedCell(int i){return MarkedCellIdList->GetId(i);};


protected:
  vtkMAFCellsFilter();
  ~vtkMAFCellsFilter();
  
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
  vtkMAFCellsFilter(const vtkMAFCellsFilter&);  // Not implemented.
  void operator=(const vtkMAFCellsFilter&);  // Not implemented.
};

#endif


