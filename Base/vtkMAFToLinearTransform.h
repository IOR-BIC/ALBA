/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFToLinearTransform.h,v $
  Language:  C++
  Date:      $Date: 2005-01-11 17:35:04 $
  Version:   $Revision: 1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkMAFToLinearTransform - convert a matrix to a transform
// .SECTION Description
// This is a very simple class which allows a vtkMatrix4x4 to be used in
// place of a vtkLinearTransform or vtkAbstractTransform.  For example,
// if you use it as a proxy between a matrix and vtkTransformPolyDataFilter
// then any modifications to the matrix will automatically be reflected in
// the output of the filter.
// .SECTION See Also
// vtkTransform vtkMatrix4x4 vtkMatrixToHomogeneousTransform 

#ifndef __vtkMAFToLinearTransform_h
#define __vtkMAFToLinearTransform_h

#include "vtkLinearTransform.h"

class vtkMatrix4x4;

class VTK_COMMON_EXPORT vtkMAFToLinearTransform : public vtkLinearTransform
{
 public:
  static vtkMAFToLinearTransform *New();
  vtkTypeRevisionMacro(vtkMAFToLinearTransform,vtkLinearTransform);
  void PrintSelf (ostream& os, vtkIndent indent);

  // Description:
  // Set the input matrix.  Any modifications to the matrix will be
  // reflected in the transformation.
  virtual void SetInput(vtkMatrix4x4*);
  vtkGetObjectMacro(Input,vtkMatrix4x4);

  // Description:
  // The input matrix is left as-is, but the transformation matrix
  // is inverted.
  void Inverse();

  // Description:
  // Get the MTime: this is the bit of magic that makes everything work.
  unsigned long GetMTime();

  // Description:
  // Make a new transform of the same type.
  vtkAbstractTransform *MakeTransform();

  // Description:
  // This method is deprecated.
  void SetMatrix(vtkMatrix4x4 *matrix) {
    this->SetInput(matrix);
    vtkWarningMacro("SetMatrix: deprecated, use SetInput() instead"); }

protected:
  vtkMAFToLinearTransform();
  ~vtkMAFToLinearTransform();

  void InternalUpdate();
  void InternalDeepCopy(vtkAbstractTransform *transform);

  int InverseFlag;
  vtkMatrix4x4 *Input;
private:
  vtkMAFToLinearTransform(const vtkMAFToLinearTransform&);  // Not implemented.
  void operator=(const vtkMAFToLinearTransform&);  // Not implemented.
};

#endif
