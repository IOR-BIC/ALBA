/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPolyDataToSinglePolyLine
 Authors: Alessandro Chiarini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAPolyDataToSinglePolyLine_h
#define __vtkALBAPolyDataToSinglePolyLine_h

#include "albaConfigure.h"
#include "vtkPolyDataAlgorithm.h"
//----------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAPolyDataToSinglePolyLine : public vtkPolyDataAlgorithm
//----------------------------------------------------------------------------
{
public:
           vtkALBAPolyDataToSinglePolyLine();
  virtual ~vtkALBAPolyDataToSinglePolyLine();
  static vtkALBAPolyDataToSinglePolyLine* New();

protected:
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

private:
	vtkALBAPolyDataToSinglePolyLine(const vtkALBAPolyDataToSinglePolyLine&);  // Not implemented.
  void operator=(const vtkALBAPolyDataToSinglePolyLine&);  // Not implemented.
};
#endif
