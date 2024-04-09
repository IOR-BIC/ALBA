/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipe
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


#include "albaDataPipe.h"

#include "albaVME.h"
#include "albaAbsMatrixPipe.h"
#include "albaOBB.h"
#include "albaEventBase.h"
#include "albaIndent.h"
//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDataPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDataPipe::albaDataPipe()
//------------------------------------------------------------------------------
{
  m_CurrentTime = 0;
  m_Bounds.Reset();
  m_VME=NULL;
  m_DependOnAbsPose=0;
  m_DependOnPose=0;
  m_DependOnVMETime = 1; //Paolo
}

//------------------------------------------------------------------------------
albaDataPipe::~albaDataPipe()
//------------------------------------------------------------------------------
{
  //this->SetVME(NULL);
  m_VME=NULL;
}

//------------------------------------------------------------------------------
void albaDataPipe::SetTimeStamp(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  if (t!=m_CurrentTime)
  {
    m_CurrentTime = t;
    Modified();
  }
}

//------------------------------------------------------------------------------
int albaDataPipe::SetVME(albaVME *vme) 
//------------------------------------------------------------------------------
{
  if (Accept(vme)||vme==NULL)
  {
    m_VME=vme;
    Modified();
    return ALBA_OK;
  }
  
  albaErrorMacro("Wrong m_VME type: " << vme->GetTypeName());
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
albaDataPipe *albaDataPipe::MakeACopy()
//------------------------------------------------------------------------------
{
  albaDataPipe *newpipe=NewInstance();
  newpipe->DeepCopy(this);
  return newpipe;
}

//------------------------------------------------------------------------------
int albaDataPipe::DeepCopy(albaDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (pipe->IsA(this->GetTypeName()))
  {
    m_VME=pipe->GetVME();
    return ALBA_OK;
  }

  return ALBA_ERROR;
}


//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
vtkMTimeType albaDataPipe::GetMTime()
//------------------------------------------------------------------------------
{
	vtkMTimeType mtime = this->albaTimeStamped::GetMTime();

  //if (m_Bounds.GetMTime() > mtime)
  //{
  //  mtime = m_Bounds.GetMTime();
  //}

  if (m_VME)
  {
    if (m_DependOnVMETime) 
    {
      unsigned long vmeMTime = m_VME->GetMTime();
      if (vmeMTime > mtime)
      {
        mtime = vmeMTime;
      }
    }

    if (m_DependOnAbsPose)
    {
      if (m_VME->GetAbsMatrixPipe())
      {
        unsigned long vme_abs_poseMTime = m_VME->GetAbsMatrixPipe()->GetMTime();
        if (vme_abs_poseMTime > mtime)
        {
          mtime = vme_abs_poseMTime;
        }
      }
    }
    else if (m_DependOnPose) // if we depend from AbsPose we do not need to check for pose
    {
      if (m_VME->GetMatrixPipe())
      {
        unsigned long vme_poseMTime = m_VME->GetMatrixPipe()->GetMTime();
        if (vme_poseMTime > mtime)
        {
          mtime = vme_poseMTime;
        }
      }
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
void albaDataPipe::PreExecute()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void albaDataPipe::Execute()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaDataPipe::OnEvent(albaEventBase *alba_event)
//------------------------------------------------------------------------------
{
  switch (alba_event->GetId())
  {
  case VME_OUTPUT_DATA_PREUPDATE: 
    if (GetMTime()>m_PreExecuteTime.GetMTime())
    {
      m_PreExecuteTime.Modified();
      PreExecute();
      // forward event to VME
      if (m_VME) m_VME->OnEvent(alba_event);
    }
  break;
  case VME_OUTPUT_DATA_UPDATE:
    Execute();
    // forward event to VME
    if (m_VME) m_VME->OnEvent(alba_event);
  break;
  }; 
}

//------------------------------------------------------------------------------
void albaDataPipe::Print(std::ostream& os, const int tabs) const
//------------------------------------------------------------------------------
{
  albaIndent indent(tabs);
  albaString dp_name;
  dp_name = m_VME ? m_VME->GetName() : "(NULL)";
  os << indent << "VME:" << dp_name.GetCStr() << std::endl;
}
