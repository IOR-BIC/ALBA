/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkALBACellLocator.h,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:33:31 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Simone Brazzale
  ========================================================================== 
  Copyright (c) 2008 CINECA
  See the COPYINGS file for license details 
  =========================================================================

  vtkALBACellLocator is equivalent to the standard vtkCellLocator
  but it implements a new function imported from VTK 5.2.
*/
#ifndef vtkALBACellLocator_h__
#define vtkALBACellLocator_h__

#include "albaConfigure.h"
#include "vtkCellLocator.h"


/**
    class name: vtkALBACellLocator
    Spatial search object to quickly locate cells in 3D.
*/

class ALBA_EXPORT vtkALBACellLocator : public vtkCellLocator
{

public:
  /** create object  instance. */
  static vtkALBACellLocator *New();
  /**  RTTI Macro. */
  vtkTypeMacro(vtkALBACellLocator, vtkCellLocator);	

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
  vtkALBACellLocator() {
  }
  /** destructor. */
  ~vtkALBACellLocator() {}
};

#endif // vtkALBACellLocator_h__

