/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAToLinearTransform.cxx,v $
  Language:  C++
  Date:      $Date: 2010-07-08 15:40:18 $
  Version:   $Revision: 1.5.4.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaMatrix.h"
#include "albaTransformBase.h"

#include "vtkALBAToLinearTransform.h"

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkALBAToLinearTransform);

//----------------------------------------------------------------------------
vtkALBAToLinearTransform::vtkALBAToLinearTransform()
{
  this->m_InputMatrix     = NULL;
  this->m_InputTransform  = NULL;
  this->m_InverseFlag     = 0;
}

//----------------------------------------------------------------------------
vtkALBAToLinearTransform::~vtkALBAToLinearTransform()
//----------------------------------------------------------------------------
{
  this->SetInputMatrix((albaMatrix *)NULL);
  this->SetInputTransform((albaTransformBase *)NULL);
}

//----------------------------------------------------------------------------
void vtkALBAToLinearTransform::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Update();

  this->Superclass::PrintSelf(os, indent);
  os << indent << "InputMatrix: " << this->m_InputMatrix << "\n";
  os << indent << "InputTransform: " << this->m_InputTransform << "\n";
  os << indent << "InverseFlag: " << this->m_InverseFlag << "\n";
}

//----------------------------------------------------------------------------
void vtkALBAToLinearTransform::SetInputMatrix(albaMatrix *mat)
//----------------------------------------------------------------------------
{
  if (mat!=m_InputMatrix)
  {
    vtkDEL(m_InputMatrix);
    m_InputMatrix=mat;
    if (mat != NULL)
    {
      mat->Register(this);
    }
    Modified();
  }
}

//----------------------------------------------------------------------------
void vtkALBAToLinearTransform::SetInputTransform(albaTransformBase *trans)
//----------------------------------------------------------------------------
{
  if (trans!=m_InputTransform)
  {
    //vtkDEL(m_InputTransform);
    m_InputTransform=trans;
    //trans->Register(this); // do not register to avoid cross-registering since this transform is created by albaTransformBase
    Modified();
  }
}

//----------------------------------------------------------------------------
void vtkALBAToLinearTransform::Inverse()
//----------------------------------------------------------------------------
{
  this->m_InverseFlag = !this->m_InverseFlag;
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkALBAToLinearTransform::InternalUpdate()
{
  if (this->m_InputTransform)
  {
    this->Matrix->DeepCopy(this->m_InputTransform->GetMatrix().GetVTKMatrix());
    if (this->m_InverseFlag)
    {
      this->Matrix->Invert();
    }
  }
  else if (this->m_InputMatrix)
  {
    this->Matrix->DeepCopy(this->m_InputMatrix->GetVTKMatrix());
    if (this->m_InverseFlag)
    {
      this->Matrix->Invert();
    }
  }
  else
  {
    this->Matrix->Identity();
  }
}

//----------------------------------------------------------------------------
void vtkALBAToLinearTransform::InternalDeepCopy(vtkAbstractTransform *gtrans)
//----------------------------------------------------------------------------
{
  vtkALBAToLinearTransform *transform = 
    (vtkALBAToLinearTransform *)gtrans;

  if (transform->GetInputMatrix())
  {
    this->SetInputMatrix(transform->m_InputMatrix);
  }
  else if (transform->GetInputTransform())
  {
    this->SetInputTransform(transform->m_InputTransform);
  }
  if (this->m_InverseFlag != transform->m_InverseFlag)
    {
    this->Inverse();
    }
}

//----------------------------------------------------------------------------
vtkAbstractTransform *vtkALBAToLinearTransform::MakeTransform()
//----------------------------------------------------------------------------
{
  return vtkALBAToLinearTransform::New();
}

//----------------------------------------------------------------------------
// Get the MTime
vtkMTimeType vtkALBAToLinearTransform::GetMTime()
//----------------------------------------------------------------------------
{
  unsigned long mtime = this->vtkLinearTransform::GetMTime();

  if (this->m_InputMatrix)
  {
    unsigned long matrixMTime = this->m_InputMatrix->GetMTime();
    if (matrixMTime > mtime)
    {
      return matrixMTime;
    }
  }
  else if (this->m_InputTransform)
  {
    unsigned long transformMTime = this->m_InputTransform->GetMTime();
    if (transformMTime > mtime)
    {
      return transformMTime;
    }
  }
  return mtime;
}
