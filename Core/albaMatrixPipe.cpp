/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixPipe
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "albaMatrixPipe.h"

#include "albaMatrix.h"
#include "albaVME.h"
#include "albaEventBase.h"
#include "albaDecl.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaMatrixPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaMatrixPipe::albaMatrixPipe()
//------------------------------------------------------------------------------
{
  m_UpdateMatrixObserverFlag=1;
  m_Updating=0; // this should be used to avoid updating loops
  m_VME=NULL;
}

//------------------------------------------------------------------------------
albaMatrixPipe::~albaMatrixPipe()
//------------------------------------------------------------------------------
{
  this->SetVME(NULL);
}

//------------------------------------------------------------------------------
void albaMatrixPipe::SetTimeStamp(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  if (t!=m_TimeStamp)
  {
    albaTransformBase::SetTimeStamp(t);Modified(); 
  }
}

//------------------------------------------------------------------------------
albaTimeStamp albaMatrixPipe::GetTimeStamp()
//------------------------------------------------------------------------------
{
  return albaTransformBase::GetTimeStamp();
}

//------------------------------------------------------------------------------
int albaMatrixPipe::SetVME(albaVME *vme)
//------------------------------------------------------------------------------
{
  if (this->Accept(vme) || vme == NULL)
  {
    m_VME=vme;
    Modified();
    return ALBA_OK;
  }

  albaErrorMacro("Wrong VME type: "<<vme->GetTypeName());
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
albaMatrixPipe *albaMatrixPipe::MakeACopy()
//------------------------------------------------------------------------------
{
  albaMatrixPipe *newpipe=NewInstance();
  newpipe->DeepCopy(this);
  return newpipe;
}

//------------------------------------------------------------------------------
int albaMatrixPipe::DeepCopy(albaMatrixPipe *pipe)
//------------------------------------------------------------------------------
{
  if (pipe->IsA(GetTypeId()))
  {
    m_VME=pipe->GetVME();
    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
const albaMatrix &albaMatrixPipe::GetMatrix()
//------------------------------------------------------------------------------
{
  if (m_Updating) // !!! to  be checked!!!
    return *m_Matrix;

  return this->Superclass::GetMatrix();
}


//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
vtkMTimeType albaMatrixPipe::GetMTime()
//------------------------------------------------------------------------------
{
	vtkMTimeType mtime = this->Superclass::GetMTime();

  if (m_VME)
  {
		vtkMTimeType vmeMTime = m_VME->GetMTime();
    if (vmeMTime > mtime)
    {
      return vmeMTime;
    }
  }
  return mtime;
}

//----------------------------------------------------------------------------
void albaMatrixPipe::InternalUpdate()
//----------------------------------------------------------------------------
{
  if (m_VME) m_VME->OnEvent(&albaEventBase(this,VME_MATRIX_UPDATE));
}

//----------------------------------------------------------------------------
void albaMatrixPipe::Update()
//----------------------------------------------------------------------------
{
//  if (m_VME) m_VME->OnEvent(&albaEventBase(this,VME_MATRIX_PREUPDATE));
  
  Superclass::Update();
}
