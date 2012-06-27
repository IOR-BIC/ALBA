/*=========================================================================

 Program: MAF2
 Module: vtkMAFPolyDataToSinglePolyLine
 Authors: Alessandro Chiarini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFPolyDataToSinglePolyLine_h
#define __vtkMAFPolyDataToSinglePolyLine_h

#include "vtkMAFConfigure.h"
#include "vtkPolyDataToPolyDataFilter.h"
//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMAFPolyDataToSinglePolyLine : public vtkPolyDataToPolyDataFilter
//----------------------------------------------------------------------------
{
public:
  //vtkTypeRevisionMacro(vtkMAFFixedCutter,vtkCutter);

           vtkMAFPolyDataToSinglePolyLine();
  virtual ~vtkMAFPolyDataToSinglePolyLine();
  static vtkMAFPolyDataToSinglePolyLine* New();

protected:
  void Execute();

private:
	vtkMAFPolyDataToSinglePolyLine(const vtkMAFPolyDataToSinglePolyLine&);  // Not implemented.
  void operator=(const vtkMAFPolyDataToSinglePolyLine&);  // Not implemented.
};
#endif
