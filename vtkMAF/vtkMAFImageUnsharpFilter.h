/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFImageUnsharpFilter.h,v $

  Copyright (c) Daniele Giunchi
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFImageUnsharpFilter - Performs a gaussian convolution.
// .SECTION Description
// vtkMAFImageUnsharpFilter implements a convolution of the input image
// with a gaussian. Supports from one to three dimensional convolutions.

#ifndef __vtkMAFImageUnsharpFilter_h
#define __vtkMAFImageUnsharpFilter_h

#include "mafConfigure.h"
#include "vtkImageToImageFilter.h"
/**
class name : vtkMAFImageUnsharpFilter.
Filter for applying unsharp to an image
*/
class MAF_EXPORT vtkMAFImageUnsharpFilter : public vtkImageToImageFilter
{
public:
  /** RTTI macro*/
  vtkTypeRevisionMacro(vtkMAFImageUnsharpFilter,vtkImageToImageFilter);
  /** print information */
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  /**Creates an instance of vtkMAFImageUnsharpFilter with the following*/
  static vtkMAFImageUnsharpFilter *New();

protected:
  /** constructor */
  vtkMAFImageUnsharpFilter();
  /** destructor */
  ~vtkMAFImageUnsharpFilter();

  /** execute filter on threads*/
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, int extent[6], int id);
  
private:
  /** copy constructor not implemented */
  vtkMAFImageUnsharpFilter(const vtkMAFImageUnsharpFilter&);
  /** operator= not implemented */
  void operator=(const vtkMAFImageUnsharpFilter&);
};

#endif










