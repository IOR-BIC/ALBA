/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFToLinearTransform.cxx,v $
  Language:  C++
  Date:      $Date: 2004-11-25 19:16:43 $
  Version:   $Revision: 1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMAFToLinearTransform.h"

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMAFToLinearTransform, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFToLinearTransform);
vtkCxxSetObjectMacro(vtkMAFToLinearTransform,Input,vtkMatrix4x4);

//----------------------------------------------------------------------------
vtkMAFToLinearTransform::vtkMAFToLinearTransform()
{
  this->Input = NULL;
  this->InverseFlag = 0;
}

//----------------------------------------------------------------------------
vtkMAFToLinearTransform::~vtkMAFToLinearTransform()
{
  this->SetInput(NULL);
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Update();

  this->Superclass::PrintSelf(os, indent);
  os << indent << "Input: " << this->Input << "\n";
  os << indent << "InverseFlag: " << this->InverseFlag << "\n";
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::Inverse()
{
  this->InverseFlag = !this->InverseFlag;
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMAFToLinearTransform::InternalUpdate()
{
  if (this->Input)
    {
    this->Matrix->DeepCopy(this->Input);
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
{
  vtkMAFToLinearTransform *transform = 
    (vtkMAFToLinearTransform *)gtrans;

  this->SetInput(transform->Input);

  if (this->InverseFlag != transform->InverseFlag)
    {
    this->Inverse();
    }
}

//----------------------------------------------------------------------------
vtkAbstractTransform *vtkMAFToLinearTransform::MakeTransform()
{
  return vtkMAFToLinearTransform::New();
}

//----------------------------------------------------------------------------
// Get the MTime
unsigned long vtkMAFToLinearTransform::GetMTime()
{
  unsigned long mtime = this->vtkLinearTransform::GetMTime();

  if (this->Input)
    {
    unsigned long matrixMTime = this->Input->GetMTime();
    if (matrixMTime > mtime)
      {
      return matrixMTime;
      }
    }
  return mtime;
}
