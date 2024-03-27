/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAFixedCutter
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAFixedCutter_h
#define __vtkALBAFixedCutter_h

#include "albaConfigure.h"
#include "vtkCutter.h"
/**
class name:  vtkALBAFixedCutter
specialization of vtkCutter in which after execution if the number of points is zero, it will added one point
that is the center of the input.
*/
class ALBA_EXPORT vtkALBAFixedCutter : public vtkCutter
{
public:
  vtkTypeMacro(vtkALBAFixedCutter,vtkCutter);
  /** constructor */
           vtkALBAFixedCutter();
  /** destructor */
  virtual ~vtkALBAFixedCutter();
  /** create an instance of the object */
  static vtkALBAFixedCutter* New();

protected:
  /** reimplement execute fixing the algorithm when the number of points of the cutter output is zero.*/
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

private:
    /** Copy Constructor , not implemented */
	vtkALBAFixedCutter(const vtkALBAFixedCutter&);
  /** operator =, not implemented */
  void operator=(const vtkALBAFixedCutter&);
};
#endif
