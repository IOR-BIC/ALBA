/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFToLinearTransform.cxx,v $
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
#include "mafMatrix.h"
#include "mafTransformBase.h"

#include "vtkMAFToLinearTransform.h"

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFToLinearTransform, "$Revision: 1.5.4.1 $");
vtkStandardNewMacro(vtkMAFToLinearTransform);

//----------------------------------------------------------------------------
vtkMAFToLinearTransform::vtkMAFToLinearTransform()
{
  this->m_InputMatrix     = NULL;
  this->m_InputTransform  = NULL;
  this->m_InverseFlag     = 0;
}

//----------------------------------------------------------------------------
vtkMAFToLinearTransform::~vtkMAFToLinearTransform()
//----------------------------------------------------------------------------
{
  this->SetInputMatrix((mafMatrix *)NULL);
  this->SetInputTransform((mafTransformBase *)NULL);
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Update();

  this->Superclass::PrintSelf(os, indent);
  os << indent << "InputMatrix: " << this->m_InputMatrix << "\n";
  os << indent << "InputTransform: " << this->m_InputTransform << "\n";
  os << indent << "InverseFlag: " << this->m_InverseFlag << "\n";
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::SetInputMatrix(mafMatrix *mat)
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
void vtkMAFToLinearTransform::SetInputTransform(mafTransformBase *trans)
//----------------------------------------------------------------------------
{
  if (trans!=m_InputTransform)
  {
    //vtkDEL(m_InputTransform);
    m_InputTransform=trans;
    //trans->Register(this); // do not register to avoid cross-registering since this transform is created by mafTransformBase
    Modified();
  }
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::Inverse()
//----------------------------------------------------------------------------
{
  this->m_InverseFlag = !this->m_InverseFlag;
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::InternalUpdate()
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
void vtkMAFToLinearTransform::InternalDeepCopy(vtkAbstractTransform *gtrans)
//----------------------------------------------------------------------------
{
  vtkMAFToLinearTransform *transform = 
    (vtkMAFToLinearTransform *)gtrans;

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
vtkAbstractTransform *vtkMAFToLinearTransform::MakeTransform()
//----------------------------------------------------------------------------
{
  return vtkMAFToLinearTransform::New();
}

//----------------------------------------------------------------------------
// Get the MTime
unsigned long vtkMAFToLinearTransform::GetMTime()
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
