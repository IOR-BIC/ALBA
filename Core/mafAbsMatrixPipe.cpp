/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAbsMatrixPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:27:15 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafAbsMatrixPipe.h"

#include "mafMatrix.h"
#include "mafVME.h"
#include "mafTransformFrame.h"

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
int mafAbsMatrixPipe::SetVME(mafVME *vme)
//----------------------------------------------------------------------------
{
  if (Superclass::SetVME(vme)==MAF_OK)
  {
    if (vme)
    {
      m_Transform->SetInput(vme->GetMatrixPipe());
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

  mafMatrixPipe *input = (mafMatrixPipe *)m_Transform->GetInput();
  mafMatrixPipe *input_frame = (mafMatrixPipe *)m_Transform->GetInputFrame();

  mafTimeStamp old_vme_time = -1;
  mafTimeStamp old_vme_parent_time = -1;


  // if the Current time of this transform is different from 
  // the input transforms' ones, force temporary input 
  // and input frame to a new current time. Also disable 
  // the rising update event since it's only a temporary 
  // update of the matrix.
  if (input)
  {
    old_vme_time=input->GetCurrentTime();

    if (m_CurrentTime!=old_vme_time)
    {
      input->SetCurrentTime(m_CurrentTime);
    }
  }

  if (input_frame)
  {
    old_vme_parent_time = input_frame->GetCurrentTime();

    if (old_vme_parent_time != m_CurrentTime)
    {
      input_frame->SetCurrentTime(m_CurrentTime); 
    }
  }
  
  if (m_UpdateMatrixObserverFlag)
  {
    InvokeEvent(MATRIX_UPDATED,m_Matrix);
  }
   
  if (input&&m_CurrentTime!=old_vme_time)
  {
    input->SetCurrentTime(old_vme_time);
  }

  if (input_frame&&old_vme_parent_time != m_CurrentTime)
  {
    input_frame->SetCurrentTime(old_vme_parent_time);
  }

  m_Updating=0;
}
