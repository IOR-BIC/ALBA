/*=========================================================================

 Program: MAF2
 Module: mafDataPipe
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafDataPipe.h"

#include "mafVME.h"
#include "mafAbsMatrixPipe.h"
#include "mafOBB.h"
#include "mafEventBase.h"
#include "mafIndent.h"
//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDataPipe)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataPipe::mafDataPipe()
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
mafDataPipe::~mafDataPipe()
//------------------------------------------------------------------------------
{
  //this->SetVME(NULL);
  m_VME=NULL;
}

//------------------------------------------------------------------------------
void mafDataPipe::SetTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  if (t!=m_CurrentTime)
  {
    m_CurrentTime = t;
    Modified();
  }
}

//------------------------------------------------------------------------------
int mafDataPipe::SetVME(mafVME *vme) 
//------------------------------------------------------------------------------
{
  if (Accept(vme)||vme==NULL)
  {
    m_VME=vme;
    Modified();
    return MAF_OK;
  }
  
  mafErrorMacro("Wrong m_VME type: " << vme->GetTypeName());
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
mafDataPipe *mafDataPipe::MakeACopy()
//------------------------------------------------------------------------------
{
  mafDataPipe *newpipe=NewInstance();
  newpipe->DeepCopy(this);
  return newpipe;
}

//------------------------------------------------------------------------------
int mafDataPipe::DeepCopy(mafDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (pipe->IsA(this->GetTypeName()))
  {
    m_VME=pipe->GetVME();
    return MAF_OK;
  }

  return MAF_ERROR;
}


//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
unsigned long mafDataPipe::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mtime = this->mafTimeStamped::GetMTime();

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
void mafDataPipe::PreExecute()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
void mafDataPipe::Execute()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafDataPipe::OnEvent(mafEventBase *maf_event)
//------------------------------------------------------------------------------
{
  switch (maf_event->GetId())
  {
  case VME_OUTPUT_DATA_PREUPDATE: 
    if (GetMTime()>m_PreExecuteTime.GetMTime())
    {
      m_PreExecuteTime.Modified();
      PreExecute();
      // forward event to VME
      if (m_VME) m_VME->OnEvent(maf_event);
    }
  break;
  case VME_OUTPUT_DATA_UPDATE:
    Execute();
    // forward event to VME
    if (m_VME) m_VME->OnEvent(maf_event);
  break;
  }; 
}

//------------------------------------------------------------------------------
void mafDataPipe::Print(std::ostream& os, const int tabs) const
//------------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  mafString dp_name;
  dp_name = m_VME ? m_VME->GetName() : "(NULL)";
  os << indent << "VME:" << dp_name.GetCStr() << std::endl;
}
