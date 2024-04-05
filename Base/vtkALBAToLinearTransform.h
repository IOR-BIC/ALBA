/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAToLinearTransform.h,v $
  Language:  C++
  Date:      $Date: 2010-07-08 15:40:18 $
  Version:   $Revision: 1.2.22.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#ifndef __vtkALBAToLinearTransform_h
#define __vtkALBAToLinearTransform_h

#include "vtkLinearTransform.h"
#include "albaConfigure.h"


class albaTransformBase;
class albaMatrix;

/** vtkALBAToLinearTransform - convert a ALBA transform or matrix into a VTK transform.
  This is a very simple class which allows a albaMatrix or albaTransform to be used in
  place of a vtkLinearTransform or vtkAbstractTransform. This allows to attach ALBA
  objects to VTK pipeline mechanisms

  @sa albaTransformBase albaMatrix vtkLinearTransform  
*/
class ALBA_EXPORT vtkALBAToLinearTransform : public vtkLinearTransform
{
 public:
  static vtkALBAToLinearTransform *New();
	vtkTypeMacro(vtkALBAToLinearTransform,vtkLinearTransform);
  void PrintSelf (ostream& os, vtkIndent indent);
  
  /** 
    Set the input matrix.  Any modifications to the ALBA matrix will be
    reflected in the VTK transformation. This also set InputTransform to NULL.*/
  virtual void SetInputMatrix(albaMatrix *);

  /** return connected matrix if exists. */
  albaMatrix *GetInputMatrix() {return m_InputMatrix;}
  
  /** 
    Set the input transform.  Any modifications to the ALBA transform will be
    reflected in the VTK transformation. This also set InputMatrix to NULL.*/
  virtual void SetInputTransform(albaTransformBase *);

  /** return connected matrix if exists. */
  albaTransformBase *GetInputTransform() {return m_InputTransform;}
 
  /**
    The input matrix is left as-is, but the transformation matrix
    is inverted. */
  void Inverse();

  /** Get the MTime: this is the bit of magic that makes everything work. */
	vtkMTimeType GetMTime();

  /** Make a new transform of the same type. */
  vtkAbstractTransform *MakeTransform();

protected:
  vtkALBAToLinearTransform();
  ~vtkALBAToLinearTransform();

  void InternalUpdate();
  void InternalDeepCopy(vtkAbstractTransform *transform);

  int               m_InverseFlag;

  albaMatrix         *m_InputMatrix;
  albaTransformBase  *m_InputTransform;
  
private:
  vtkALBAToLinearTransform(const vtkALBAToLinearTransform&);  // Not implemented.
  void operator=(const vtkALBAToLinearTransform&);  // Not implemented.
};

#endif
