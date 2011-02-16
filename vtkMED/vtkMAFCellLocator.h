/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFCellLocator.h,v $ 
  Language: C++ 
  Date: $Date: 2011-02-16 21:15:02 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Simone Brazzale
  ========================================================================== 
  Copyright (c) 2008 CINECA
  See the COPYINGS file for license details 
  =========================================================================

  vtkMAFCellLocator is equivalent to the standard vtkCellLocator
  but it implements a new function imported from VTK 5.2.
*/
#ifndef vtkMAFCellLocator_h__
#define vtkMAFCellLocator_h__

#include "vtkCellLocator.h"
#include "vtkMAFConfigure.h"

/**
    class name: vtkMAFCellLocator
    Spatial search object to quickly locate cells in 3D.
*/

class vtkMAFCellLocator : public vtkCellLocator
{

public:
  /** create object  instance. */
  static vtkMAFCellLocator *New();
  /**  RTTI Macro. */
  vtkTypeRevisionMacro(vtkMAFCellLocator, vtkCellLocator);	

public:

  // Description:
  // Given a finite line defined by the two points (p1,p2), return the list
  // of unique cell ids in the buckets containing the line. It is possible
  // that an empty cell list is returned. The user must provide the vtkIdList
  // to populate. This method returns data only after the locator has been
  // built.
  void FindCellsAlongLine(double p1[3], double p2[3], double tolerance,
                          vtkIdList *cells);

protected:
  /** constructor */
  vtkMAFCellLocator() {
  }
  /** destructor. */
  ~vtkMAFCellLocator() {}
};

#endif // vtkMAFCellLocator_h__