/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFToLinearTransform.h,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:21:39 $
  Version:   $Revision: 1.2 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __vtkMAFToLinearTransform_h
#define __vtkMAFToLinearTransform_h

#include "vtkLinearTransform.h"

class mafTransformBase;
class mafMatrix;

/** vtkMAFToLinearTransform - convert a MAF transform or matrix into a VTK transform.
  This is a very simple class which allows a mafMatrix or mafTransform to be used in
  place of a vtkLinearTransform or vtkAbstractTransform. This allows to attach MAF
  objects to VTK pipeline mechanisms

  @sa mafTransformBase mafMatrix vtkLinearTransform  
*/
class MAF_EXPORT vtkMAFToLinearTransform : public vtkLinearTransform
{
 public:
  static vtkMAFToLinearTransform *New();
  vtkTypeRevisionMacro(vtkMAFToLinearTransform,vtkLinearTransform);
  void PrintSelf (ostream& os, vtkIndent indent);
  
  /** 
    Set the input matrix.  Any modifications to the MAF matrix will be
    reflected in the VTK transformation. This also set InputTransform to NULL.*/
  virtual void SetInputMatrix(mafMatrix *);

  /** return connected matrix if exists. */
  mafMatrix *GetInputMatrix() {return InputMatrix;}
  
  /** 
    Set the input transform.  Any modifications to the MAF transform will be
    reflected in the VTK transformation. This also set InputMatrix to NULL.*/
  virtual void SetInputTransform(mafTransformBase *);

  /** return connected matrix if exists. */
  mafTransformBase *GetInputTransform() {return InputTransform;}
 
  /**
    The input matrix is left as-is, but the transformation matrix
    is inverted. */
  void Inverse();

  /** Get the MTime: this is the bit of magic that makes everything work. */
  unsigned long GetMTime();

  /** Make a new transform of the same type. */
  vtkAbstractTransform *MakeTransform();

protected:
  vtkMAFToLinearTransform();
  ~vtkMAFToLinearTransform();

  void InternalUpdate();
  void InternalDeepCopy(vtkAbstractTransform *transform);

  int               InverseFlag;

  mafMatrix         *InputMatrix;
  mafTransformBase  *InputTransform;
  
private:
  vtkMAFToLinearTransform(const vtkMAFToLinearTransform&);  // Not implemented.
  void operator=(const vtkMAFToLinearTransform&);  // Not implemented.
};

#endif
