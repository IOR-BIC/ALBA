/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: mafDataPipe.cpp,v $
Language:  C++
Date:      $Date: 2005-03-11 10:10:46 $
Version:   $Revision: 1.1 $



=========================================================================*/
#include "mafDataPipe.h"

#include "mafVME.h"
#include "mafVMEItem.h"
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
  m_CurrentBounds.Reset();
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
int mafDataPipe::SetVME(mafVME *vme) 
//------------------------------------------------------------------------------
{
  if (Accept(vme)||vme==NULL)
  {
    m_VME=vme;
    Modified();
    return MAF_OK;
  }
  
  mafErrorMacro("Wrong m_VME type: " << vme->GetClassName());
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
  if (pipe->IsA(this->GetClassName()))
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

  //if (m_CurrentBounds.GetMTime() > mtime)
  //{
  //  mtime = m_CurrentBounds.GetMTime();
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
// TO BE rewritten
vtkDataSet * mafDataPipe::GetOutput() 
//------------------------------------------------------------------------------
{
/*  // check to see if an execute is necessary.
  if (this->Outputs && this->Outputs[0])
  {
    return (vtkDataSet *)(this->Outputs[0]);
  }

  if (this->Outputs == NULL)
  {
    // this should create the right object type in the Output array
    //this->ExecuteInformation();
    this->UpdateInformation();
  }

  if (this->Outputs == NULL)
  {
    return NULL;
  }

  if (this->Outputs[0])
  {
    return (vtkDataSet *)(this->Outputs[0]);
  }
*/
  return NULL;
}

//------------------------------------------------------------------------------
void mafDataPipe::UpdateBounds()
//------------------------------------------------------------------------------
{
  //m_CurrentBounds.DeepCopy(this->GetOutput()->GetBounds());
}

//------------------------------------------------------------------------------
void mafDataPipe::Print(std::ostream& os, const int tabs)
//------------------------------------------------------------------------------
{
  
}