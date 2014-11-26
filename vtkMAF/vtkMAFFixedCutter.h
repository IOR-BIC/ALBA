/*=========================================================================

 Program: MAF2
 Module: vtkMAFFixedCutter
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFFixedCutter_h
#define __vtkMAFFixedCutter_h

#include "mafConfigure.h"
#include "vtkCutter.h"
/**
class name:  vtkMAFFixedCutter
specialization of vtkCutter in which after execution if the number of points is zero, it will added one point
that is the center of the input.
*/
class MAF_EXPORT vtkMAFFixedCutter : public vtkCutter
{
public:
  vtkTypeRevisionMacro(vtkMAFFixedCutter,vtkCutter);
  /** constructor */
           vtkMAFFixedCutter();
  /** destructor */
  virtual ~vtkMAFFixedCutter();
  /** create an instance of the object */
  static vtkMAFFixedCutter* New();

protected:
  /** reimplement execute fixing the algorithm when the number of points of the cutter output is zero.*/
  void Execute();

private:
    /** Copy Constructor , not implemented */
	vtkMAFFixedCutter(const vtkMAFFixedCutter&);
  /** operator =, not implemented */
  void operator=(const vtkMAFFixedCutter&);
};
#endif
