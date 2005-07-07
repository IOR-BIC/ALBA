/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrixInterpolator.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-07 15:24:37 $
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


#include "mafMatrixInterpolator.h"

#include "mafVME.h"
#include "mafMatrix.h"
#include "mafVMEGenericAbstract.h"

#include "mafMatrixVector.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafMatrixInterpolator)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafMatrixInterpolator::mafMatrixInterpolator()
//------------------------------------------------------------------------------
{
  m_CurrentItem   = NULL;
  m_OldTimeStamp  = -1;
  m_OldItem       = NULL;
}

//------------------------------------------------------------------------------
mafMatrixInterpolator::~mafMatrixInterpolator()
//------------------------------------------------------------------------------
{
  SetCurrentItem(NULL);
}

//----------------------------------------------------------------------------
void mafMatrixInterpolator::SetTimeStamp(mafTimeStamp time)
//----------------------------------------------------------------------------
{
  mafTransformBase::SetTimeStamp(time);
  
  // Do not call Modified(), to avoid the pipeline
  // to be automatically updated
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
unsigned long mafMatrixInterpolator::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mtime = Superclass::GetMTime();
  
  mafVMEGenericAbstract *vme = (mafVMEGenericAbstract *)m_VME;

  if (vme && vme->GetMatrixVector())
  {
    unsigned long arrayMTime = vme->GetMatrixVector()->GetMTime();
    if (arrayMTime > mtime)
    {
      return arrayMTime;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
bool mafMatrixInterpolator::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return vme && vme->IsA(mafVMEGenericAbstract::GetStaticTypeId());
}

//------------------------------------------------------------------------------
void mafMatrixInterpolator::Update()
//------------------------------------------------------------------------------
{

  unsigned long mtime=m_UpdateTime.GetMTime();

  // if the current time has changed or if this object has been modified...
  if (m_OldTimeStamp!=GetTimeStamp() || !m_CurrentItem || (m_CurrentItem->GetMTime()>mtime))
  {
    m_OldTimeStamp=GetTimeStamp();    
    
    // First of all find the right item to be used as input
    // and update the output bounds
    InternalItemUpdate();
  }
  
  Superclass::Update();
}

//------------------------------------------------------------------------------
void mafMatrixInterpolator::InternalItemUpdate()
//------------------------------------------------------------------------------
{  
  mafVMEGenericAbstract *vme=(mafVMEGenericAbstract *)m_VME;
  mafMatrixVector *array = vme ? vme->GetMatrixVector() : NULL;

  if (array)
  {
    mafMatrix *item = array->GetItemBefore(GetTimeStamp());
    UpdateCurrentItem(item);
  }
  
}

//-------------------------------------------------------------------------
void mafMatrixInterpolator::SetCurrentItem(mafMatrix *data)
//------------------------------------------------------------------------------
{
  m_CurrentItem=data;
  Modified(); // change modification time to force internal update
}

//-------------------------------------------------------------------------
void mafMatrixInterpolator::UpdateCurrentItem(mafMatrix *item)
//------------------------------------------------------------------------------
{
  if (item)
  {	
    if (item!=m_CurrentItem)
    {
      SetCurrentItem(item);
    }
    m_OldItem=m_CurrentItem;		
  }
  else
  {
	  SetCurrentItem(NULL);
  }

  

}

//-------------------------------------------------------------------------
void mafMatrixInterpolator::InternalUpdate()
//-------------------------------------------------------------------------
{
  //InternalItemUpdate(); already called by Update
  if (m_CurrentItem)
  {
    m_Matrix->DeepCopy(GetCurrentItem());
  }
  else
  {
    m_Matrix->Identity();
  }

  m_Matrix->SetTimeStamp(GetTimeStamp());
}
