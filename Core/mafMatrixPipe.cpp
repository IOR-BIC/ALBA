/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrixPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:27:15 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafMatrixPipe.h"

#include "mafMatrix.h"
#include "mafVME.h"
#include "mafEventBase.h"
#include "mafDecl.h"
#include <sstream>

//------------------------------------------------------------------------------
mafMatrixPipe::mafMatrixPipe()
//------------------------------------------------------------------------------
{
  m_UpdateMatrixObserverFlag=1;
  m_Updating=0; // this should be used to avoid updating loops
  m_VME=NULL;
  m_CurrentTime=0;
  
}

//------------------------------------------------------------------------------
mafMatrixPipe::~mafMatrixPipe()
//------------------------------------------------------------------------------
{
  this->SetVME(NULL);
}

//------------------------------------------------------------------------------
int mafMatrixPipe::SetVME(mafVME *vme)
//------------------------------------------------------------------------------
{
  if (this->Accept(vme) || vme == NULL)
  {
    m_VME=vme;
    Modified();
    return MAF_OK;
  }

  mafErrorMacro("Wrong VME type: "<<vme->GetTypeName());
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
mafMatrixPipe *mafMatrixPipe::MakeACopy()
//------------------------------------------------------------------------------
{
  mafMatrixPipe *newpipe=NewInstance();
  newpipe->DeepCopy(this);
  return newpipe;
}

//------------------------------------------------------------------------------
int mafMatrixPipe::DeepCopy(mafMatrixPipe *pipe)
//------------------------------------------------------------------------------
{
  if (pipe->IsA(GetTypeId()))
  {
    m_VME=pipe->GetVME();
    return MAF_OK;
  }

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
const mafMatrix &mafMatrixPipe::GetMatrix()
//------------------------------------------------------------------------------
{
  if (m_Updating) // !!! to  be checked!!!
    return *m_Matrix;

  return this->Superclass::GetMatrix();
}


//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
unsigned long mafMatrixPipe::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mtime = this->Superclass::GetMTime();

  if (m_VME)
  {
    unsigned long vmeMTime = m_VME->GetMTime();
    if (vmeMTime > mtime)
    {
      return vmeMTime;
    }
  }
  return mtime;
}

//----------------------------------------------------------------------------
void mafMatrixPipe::InternalUpdate()
//----------------------------------------------------------------------------
{
  InvokeEvent(MATRIX_UPDATED);
}
