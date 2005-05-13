/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFToLinearTransform.cxx,v $
  Language:  C++
  Date:      $Date: 2005-05-13 16:53:20 $
  Version:   $Revision: 1.4 $

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

vtkCxxRevisionMacro(vtkMAFToLinearTransform, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkMAFToLinearTransform);

//----------------------------------------------------------------------------
vtkMAFToLinearTransform::vtkMAFToLinearTransform()
{
  this->InputMatrix     = NULL;
  this->InputTransform  = NULL;
  this->InverseFlag     = 0;
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
  os << indent << "InputMatrix: " << this->InputMatrix << "\n";
  os << indent << "InputTransform: " << this->InputTransform << "\n";
  os << indent << "InverseFlag: " << this->InverseFlag << "\n";
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::SetInputMatrix(mafMatrix *mat)
//----------------------------------------------------------------------------
{
  if (mat!=InputMatrix)
  {
    vtkDEL(InputMatrix);
    InputMatrix=mat;
    mat->Register(this);
    Modified();
  }
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::SetInputTransform(mafTransformBase *trans)
//----------------------------------------------------------------------------
{
  if (trans!=InputTransform)
  {
    //vtkDEL(InputTransform);
    InputTransform=trans;
    //trans->Register(this); // do not register to avoid cross-registering since this transform is created by mafTransformBase
    Modified();
  }
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::Inverse()
//----------------------------------------------------------------------------
{
  this->InverseFlag = !this->InverseFlag;
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::InternalUpdate()
{
  if (this->InputTransform)
  {
    this->Matrix->DeepCopy(this->InputTransform->GetMatrix().GetVTKMatrix());
    if (this->InverseFlag)
    {
      this->Matrix->Invert();
    }
  }
  else if (this->InputMatrix)
  {
    this->Matrix->DeepCopy(this->InputMatrix->GetVTKMatrix());
    if (this->InverseFlag)
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
    this->SetInputMatrix(transform->InputMatrix);
  }
  else if (transform->GetInputTransform())
  {
    this->SetInputTransform(transform->InputTransform);
  }
  if (this->InverseFlag != transform->InverseFlag)
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

  if (this->InputMatrix)
  {
    unsigned long matrixMTime = this->InputMatrix->GetMTime();
    if (matrixMTime > mtime)
    {
      return matrixMTime;
    }
  }
  else if (this->InputTransform)
  {
    unsigned long transformMTime = this->InputTransform->GetMTime();
    if (transformMTime > mtime)
    {
      return transformMTime;
    }
  }
  return mtime;
}
