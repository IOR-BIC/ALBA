/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAbsMatrixPipe
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


#include "albaAbsMatrixPipe.h"
#include "albaEventBase.h"
#include "albaMatrix.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaTransformFrame.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaAbsMatrixPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaAbsMatrixPipe::albaAbsMatrixPipe()
//------------------------------------------------------------------------------
{  
  albaNEW(m_Transform);
}

//------------------------------------------------------------------------------
albaAbsMatrixPipe::~albaAbsMatrixPipe()
//------------------------------------------------------------------------------
{
  SetVME(NULL);
  albaDEL(m_Transform);
}

//----------------------------------------------------------------------------
vtkMTimeType albaAbsMatrixPipe::GetMTime()
//----------------------------------------------------------------------------
{
	vtkMTimeType mtime = this->Superclass::GetMTime();

	vtkMTimeType transMTime = m_Transform->GetMTime();
  if (transMTime > mtime)
  {
    return transMTime;
  }
  
  return mtime;
}

//----------------------------------------------------------------------------
int albaAbsMatrixPipe::SetVME(albaVME *vme)
//----------------------------------------------------------------------------
{
  if (Superclass::SetVME(vme)==ALBA_OK)
  {
    if (vme)
    {
      if (vme->GetOutput())
        m_Transform->SetInput(vme->GetOutput()->GetTransform());
      else
        m_Transform->SetInput((albaTransformBase *)NULL);
      
      if (vme->GetParent())
      {
        m_Transform->SetInputFrame(vme->GetParent()->GetAbsMatrixPipe());
      }
      else
      {
        m_Transform->SetInputFrame((albaTransformBase *)NULL);
      }
    }
    else
    {
      m_Transform->SetInput((albaTransformBase *)NULL);
      m_Transform->SetInputFrame((albaTransformBase *)NULL);
    }

    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//----------------------------------------------------------------------------
void albaAbsMatrixPipe::InternalUpdate()
//----------------------------------------------------------------------------
{
  // used to avoid loops
  if (m_Updating) // !!! check the necessity of this flag !!!
    return;
  
  m_Updating=1;

  albaTransformBase *input = albaTransformBase::SafeDownCast(m_Transform->GetInput());
  albaTransformBase *input_frame = albaTransformBase::SafeDownCast(m_Transform->GetInputFrame());

  albaTimeStamp old_vme_time = -1;
  albaTimeStamp old_vme_parent_time = -1;

  m_Transform->SetTimeStamp(GetTimeStamp());

  // if the Current time of this transform is different from 
  // the input transforms' ones, force temporary input 
  // and input frame to a new current time. Also disable 
  // the rising of update event since it's only a temporary 
  // update of the matrix. This is not an optimal solution, 
  // since it will make matrix pipes' modification time to 
  // be updated, therefore all depending process objects
  // will be forced to updated themselves...
  if (input)
  {
    old_vme_time=input->GetTimeStamp();

    if (GetTimeStamp()!=old_vme_time)
    {
      input->SetTimeStamp(GetTimeStamp());
    }
  }

  if (input_frame)
  {
    old_vme_parent_time = input_frame->GetTimeStamp();

    if (old_vme_parent_time != GetTimeStamp())
    {
      input_frame->SetTimeStamp(GetTimeStamp()); 
    }
  }
  
  *m_Matrix = m_Transform->GetMatrix();

  m_Matrix->SetTimeStamp(GetTimeStamp());

  if (m_UpdateMatrixObserverFlag)
  {
    if (m_VME) m_VME->OnEvent(&albaEventBase(this,VME_MATRIX_UPDATE,m_Matrix));
  }
   
  if (input&&GetTimeStamp()!=old_vme_time)
  {
    input->SetTimeStamp(old_vme_time);
  }

  if (input_frame&&old_vme_parent_time != GetTimeStamp())
  {
    input_frame->SetTimeStamp(old_vme_parent_time);
  }

  m_Updating=0;
}
