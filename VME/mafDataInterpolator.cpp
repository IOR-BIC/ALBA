/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataInterpolator.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:16:31 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafDataInterpolator.h"

#include "mafVME.h"
#include "mafVMEItem.h"
#include "mafVMEGeneric.h"

#include "mafDataVector.h"

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafDataInterpolator)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDataInterpolator::mafDataInterpolator()
//------------------------------------------------------------------------------
{
  m_CurrentItem   = NULL;
  m_OldTimeStamp  = -1;
  m_OldItem       = NULL;
}

//------------------------------------------------------------------------------
mafDataInterpolator::~mafDataInterpolator()
//------------------------------------------------------------------------------
{
  SetCurrentItem(NULL);
}

//----------------------------------------------------------------------------
void mafDataInterpolator::SetCurrentTime(mafTimeStamp time)
//----------------------------------------------------------------------------
{
  if (m_CurrentTime==time)
    return;

  m_CurrentTime=time;
  // Do not call Modified(), to avoid the pipeline
  // to be automatically updated
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
unsigned long mafDataInterpolator::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mtime = Superclass::GetMTime();
  
  mafVMEGeneric *vme = (mafVMEGeneric *)m_VME;

  if (vme && vme->GetDataVector())
  {
    unsigned long arrayMTime = vme->GetDataVector()->GetMTime();
    if (arrayMTime > mtime)
    {
      return arrayMTime;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
bool mafDataInterpolator::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return vme && vme->IsA(mafVMEGeneric::GetStaticTypeId());
}

//------------------------------------------------------------------------------
void mafDataInterpolator::PreExecute()
//------------------------------------------------------------------------------
{
  Superclass::PreExecute();

  unsigned long mtime=GetMTime();

  // If the current time has changed, check if
  // a new item should be considered according to
  // interpolation rules (InternalItemUpdate() )

  // the current time has changed or if output data has been regenerated...
  if (m_OldTimeStamp!=m_CurrentTime||mtime>m_UpdateTime.GetMTime())
  {
    m_OldTimeStamp=m_CurrentTime;
    
    // First of all find the right item to be used as input
    // and update the output bounds
    UpdateBounds();
  } 
}

//------------------------------------------------------------------------------
void mafDataInterpolator::UpdateBounds()
//------------------------------------------------------------------------------
{
  this->InternalItemUpdate();

  if (m_CurrentItem)
  {
    if ( (!m_Bounds.IsValid())||(m_CurrentItem!=m_OldItem)|| \
      (m_CurrentItem->GetMTime()>m_Bounds.GetMTime())|| \
      (m_CurrentItem && m_CurrentItem->GetUpdateTime()>m_Bounds.GetMTime()))
    {
        m_Bounds.DeepCopy(m_CurrentItem->GetBounds());
    }
  }
}

//------------------------------------------------------------------------------
void mafDataInterpolator::InternalItemUpdate()
//------------------------------------------------------------------------------
{  
  mafVMEGeneric *vme=(mafVMEGeneric *)m_VME;
  mafDataVector *array = vme ? vme->GetDataVector() : NULL;

  if (array)
  {
    mafVMEItem *item = array->GetItemBefore(m_CurrentTime);
    UpdateCurrentItem(item);
  }
  
}

//-------------------------------------------------------------------------
void mafDataInterpolator::SetCurrentItem(mafVMEItem *data)
//------------------------------------------------------------------------------
{
  m_CurrentItem=data;
  Modified();
}

//-------------------------------------------------------------------------
void mafDataInterpolator::UpdateCurrentItem(mafVMEItem *item)
//------------------------------------------------------------------------------
{
  if (item)
  {	
    if (item!=m_CurrentItem)
    {
      SetCurrentItem(item);
      //m_UpdateTime.Modified();
    }
    m_OldItem=m_CurrentItem;
  }
  else
  {
	  SetCurrentItem(NULL);
    //m_UpdateTime.Modified();
  }

  

}