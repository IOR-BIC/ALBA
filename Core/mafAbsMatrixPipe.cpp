/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAbsMatrixPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:03:30 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafAbsMatrixPipe.h"
#include "mafEventBase.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafTransformFrame.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafAbsMatrixPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAbsMatrixPipe::mafAbsMatrixPipe()
//------------------------------------------------------------------------------
{  
  mafNEW(m_Transform);
}

//------------------------------------------------------------------------------
mafAbsMatrixPipe::~mafAbsMatrixPipe()
//------------------------------------------------------------------------------
{
  SetVME(NULL);
  mafDEL(m_Transform);
}

//----------------------------------------------------------------------------
unsigned long mafAbsMatrixPipe::GetMTime()
//----------------------------------------------------------------------------
{
  unsigned long mtime = this->Superclass::GetMTime();

  unsigned long transMTime = m_Transform->GetMTime();
  if (transMTime > mtime)
  {
    return transMTime;
  }
  
  return mtime;
}

//----------------------------------------------------------------------------
int mafAbsMatrixPipe::SetVME(mafVME *vme)
//----------------------------------------------------------------------------
{
  if (Superclass::SetVME(vme)==MAF_OK)
  {
    if (vme)
    {
      if (vme->GetOutput())
        m_Transform->SetInput(vme->GetOutput()->GetTransform());
      else
        m_Transform->SetInput((mafTransformBase *)NULL);
      
      if (vme->GetParent())
      {
        m_Transform->SetInputFrame(vme->GetParent()->GetAbsMatrixPipe());
      }
      else
      {
        m_Transform->SetInputFrame((mafTransformBase *)NULL);
      }
    }
    else
    {
      m_Transform->SetInput((mafTransformBase *)NULL);
      m_Transform->SetInputFrame((mafTransformBase *)NULL);
    }

    return MAF_OK;
  }

  return MAF_ERROR;
}

//----------------------------------------------------------------------------
void mafAbsMatrixPipe::InternalUpdate()
//----------------------------------------------------------------------------
{
  // used to avoid loops
  if (m_Updating) // !!! check the necessity of this flag !!!
    return;
  
  m_Updating=1;

  mafMatrixPipe *input = mafMatrixPipe::SafeDownCast(m_Transform->GetInput());
  mafMatrixPipe *input_frame = mafMatrixPipe::SafeDownCast(m_Transform->GetInputFrame());

  mafTimeStamp old_vme_time = -1;
  mafTimeStamp old_vme_parent_time = -1;

  m_Transform->SetTimeStamp(GetCurrentTime());

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
    old_vme_time=input->GetCurrentTime();

    if (GetCurrentTime()!=old_vme_time)
    {
      input->SetCurrentTime(GetCurrentTime());
    }
  }

  if (input_frame)
  {
    old_vme_parent_time = input_frame->GetCurrentTime();

    if (old_vme_parent_time != GetCurrentTime())
    {
      input_frame->SetCurrentTime(GetCurrentTime()); 
    }
  }
  
  *m_Matrix = m_Transform->GetMatrix();


  if (m_UpdateMatrixObserverFlag)
  {
    if (m_VME) m_VME->OnEvent(&mafEventBase(this,VME_MATRIX_UPDATE,m_Matrix));
  }
   
  if (input&&GetCurrentTime()!=old_vme_time)
  {
    input->SetCurrentTime(old_vme_time);
  }

  if (input_frame&&old_vme_parent_time != GetCurrentTime())
  {
    input_frame->SetCurrentTime(old_vme_parent_time);
  }

  m_Updating=0;
}
