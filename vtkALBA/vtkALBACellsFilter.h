/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkALBACellsFilter.h
  Language:  C++
  RCS:   $Id: vtkALBACellsFilter.h,v 1.1.2.2 2009-11-17 09:32:20 aqd0 Exp $

  Copyright (c) Goodwin Lawlor 2004
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

  Some modifications by Stefano Perticoni in order to make it work
  under ALBA

=========================================================================*/

#ifndef __vtkALBACellsFilter_h
#define __vtkALBACellsFilter_h

#include "albaConfigure.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkIdList.h"
#include "vtkCharArray.h"
#include "vtkPolyData.h"

class vtkLookupTable;

/** 
class name : vtkALBACellsFilter.
 vtkALBACellsFilter- superclass for selecting cells.
 
 vtkALBACellsFilter is a superclass for classes to perform operations on selected cells.

 @sa
 vtkALBARemoveCellsFilter
*/ 
class ALBA_EXPORT vtkALBACellsFilter : public vtkPolyDataAlgorithm
{

public:
  /** create an instance of the object */
  static vtkALBACellsFilter *New();
  /** RTTI Macro */
  vtkTypeMacro(vtkALBACellsFilter, vtkPolyDataAlgorithm);
  /** Print Object Information */
  void PrintSelf(ostream& os, vtkIndent indent);
   
 /** Mark a cell */ 
 void MarkCell(vtkIdType cellid);
 
 /** Unmark any cell that has been marked already */
 void UnmarkCell(vtkIdType cellid);
 
 /** If a cells is unmarked- mark it... if its marked- unmark it. */
 void ToggleCell(vtkIdType cellid); 
 
 /** Undo all the marks made on cells */
 void UndoMarks(){this->InitializeScalars(); this->MarkedCellIdList->Reset(); this->Scalars->Modified();}

  /** Set the marked color... default is red */
  void SetMarkedColor(double r, double g, double b);
  /** Set the marked color... default is red */
  void SetMarkedColor(double rgb[3]) {this->SetMarkedColor(rgb[0], rgb[1], rgb[2]);};
  /** Get the marked color... default is red */
  vtkGetVector3Macro(MarkedColor, double);

  /** Set the umarked color... default is white. You could set this to the same color as the vtkProperty
  and users won't know they are seeing scalars */
  void SetUnmarkedColor(double r, double g, double b);
/** Set the umarked color... default is white. You could set this to the same color as the vtkProperty
  and users won't know they are seeing scalars */
  void SetUnmarkedColor(double rgb[3]) {this->SetUnmarkedColor(rgb[0], rgb[1], rgb[2]);};
/** Get the umarked color... default is white. You could set this to the same color as the vtkProperty
  and users won't know they are seeing scalars */
  vtkGetVector3Macro(UnmarkedColor, double);

  /** Set the marked opacity... default is 1.0 By setting this to 0.0 you can temporally remove the cells
  and then permanently remove them with RemoveMarkedCells */
  void SetMarkedOpacity(double opacity);
/** Get the marked opacity... default is 1.0 By setting this to 0.0 you can temporally remove the cells
  and then permanently remove them with RemoveMarkedCells */
  vtkGetMacro(MarkedOpacity, double);
  
  /** Get the number of marked cells */
	int GetNumberOfMarkedCells(){return MarkedCellIdList->GetNumberOfIds();};

  /** Get the marked cell id from the MarkedCellIdList */
	vtkIdType GetIdMarkedCell(int i){return MarkedCellIdList->GetId(i);};


protected:
  /** constructor */
  vtkALBACellsFilter();
  /** destructor */
  ~vtkALBACellsFilter();
  
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
  
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

private:
  /** Copy Constructor , not implemented.*/
  vtkALBACellsFilter(const vtkALBACellsFilter&);
  /** operator =, not implemented */
  void operator=(const vtkALBACellsFilter&);
};

#endif


