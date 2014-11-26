/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMEDImageUnsharpFilter.h,v $

  Copyright (c) Daniele Giunchi
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMEDImageUnsharpFilter - Performs a gaussian convolution.
// .SECTION Description
// vtkMEDImageUnsharpFilter implements a convolution of the input image
// with a gaussian. Supports from one to three dimensional convolutions.

#ifndef __vtkMEDImageUnsharpFilter_h
#define __vtkMEDImageUnsharpFilter_h

#include "mafConfigure.h"
#include "vtkImageToImageFilter.h"
/**
class name : vtkMEDImageUnsharpFilter.
Filter for applying unsharp to an image
*/
class MAF_EXPORT vtkMEDImageUnsharpFilter : public vtkImageToImageFilter
{
public:
  /** RTTI macro*/
  vtkTypeRevisionMacro(vtkMEDImageUnsharpFilter,vtkImageToImageFilter);
  /** print information */
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  /**Creates an instance of vtkMEDImageUnsharpFilter with the following*/
  static vtkMEDImageUnsharpFilter *New();

protected:
  /** constructor */
  vtkMEDImageUnsharpFilter();
  /** destructor */
  ~vtkMEDImageUnsharpFilter();

  /** execute filter on threads*/
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, int extent[6], int id);
  
private:
  /** copy constructor not implemented */
  vtkMEDImageUnsharpFilter(const vtkMEDImageUnsharpFilter&);
  /** operator= not implemented */
  void operator=(const vtkMEDImageUnsharpFilter&);
};

#endif










