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

#include "mafConfigure.h"
#include "vtkPolyDataAlgorithm.h"
//----------------------------------------------------------------------------
class MAF_EXPORT vtkMAFPolyDataToSinglePolyLine : public vtkPolyDataAlgorithm
//----------------------------------------------------------------------------
{
public:
           vtkMAFPolyDataToSinglePolyLine();
  virtual ~vtkMAFPolyDataToSinglePolyLine();
  static vtkMAFPolyDataToSinglePolyLine* New();

protected:
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

private:
	vtkMAFPolyDataToSinglePolyLine(const vtkMAFPolyDataToSinglePolyLine&);  // Not implemented.
  void operator=(const vtkMAFPolyDataToSinglePolyLine&);  // Not implemented.
};
#endif
