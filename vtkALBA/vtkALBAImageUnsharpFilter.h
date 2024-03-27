/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAImageUnsharpFilter.h,v $

  Copyright (c) Daniele Giunchi
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkALBAImageUnsharpFilter - Performs a gaussian convolution.
// .SECTION Description
// vtkALBAImageUnsharpFilter implements a convolution of the input image
// with a gaussian. Supports from one to three dimensional convolutions.

#ifndef __vtkALBAImageUnsharpFilter_h
#define __vtkALBAImageUnsharpFilter_h

#include "albaConfigure.h"
#include "vtkImageAlgorithm.h"
/**
class name : vtkALBAImageUnsharpFilter.
Filter for applying unsharp to an image
*/
class ALBA_EXPORT vtkALBAImageUnsharpFilter : public vtkImageAlgorithm
{
public:
  /** RTTI macro*/
  vtkTypeMacro(vtkALBAImageUnsharpFilter,vtkImageAlgorithm);
  /** print information */
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  /**Creates an instance of vtkALBAImageUnsharpFilter with the following*/
  static vtkALBAImageUnsharpFilter *New();

protected:
  /** constructor */
  vtkALBAImageUnsharpFilter();
  /** destructor */
  ~vtkALBAImageUnsharpFilter();

  /** execute filter on threads*/
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, int extent[6], int id);
  
private:
  /** copy constructor not implemented */
  vtkALBAImageUnsharpFilter(const vtkALBAImageUnsharpFilter&);
  /** operator= not implemented */
  void operator=(const vtkALBAImageUnsharpFilter&);
};

#endif
