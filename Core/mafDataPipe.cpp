/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataPipe.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:57:38 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
}

//------------------------------------------------------------------------------
mafDataPipe::~mafDataPipe()
//------------------------------------------------------------------------------
{
  //this->SetVME(NULL);
  m_VME=NULL;
}

//------------------------------------------------------------------------------
void mafDataPipe::SetCurrentTime(mafTimeStamp t)
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
    unsigned long vmeMTime = m_VME->GetMTime();
    if (vmeMTime > mtime)
    {
      mtime = vmeMTime;
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
void mafDataPipe::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  if (m_VME) m_VME->OnEvent(event);
}

//------------------------------------------------------------------------------
void mafDataPipe::Print(std::ostream& os, const int tabs) const
//------------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  os << indent << "VME:" << (m_VME?m_VME->GetName():"(NULL)") << std::endl;
}