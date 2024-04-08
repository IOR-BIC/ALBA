/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolator
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


#include "albaDataPipeInterpolator.h"

#include "albaVME.h"
#include "albaVMEItem.h"
#include "albaVMEGenericAbstract.h"

#include "albaDataVector.h"

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaDataPipeInterpolator)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaDataPipeInterpolator::albaDataPipeInterpolator()
//------------------------------------------------------------------------------
{
  m_CurrentItem     = NULL;
  m_OldTimeStamp    = -1;
  m_OldItem         = NULL;
  m_ReleaseDataFlag = false;
  m_DependOnVMETime = 0;
}

//------------------------------------------------------------------------------
albaDataPipeInterpolator::~albaDataPipeInterpolator()
//------------------------------------------------------------------------------
{
  SetCurrentItem(NULL);
}

//----------------------------------------------------------------------------
void albaDataPipeInterpolator::SetTimeStamp(albaTimeStamp time)
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
vtkMTimeType albaDataPipeInterpolator::GetMTime()
//------------------------------------------------------------------------------
{
	vtkMTimeType mtime = Superclass::GetMTime();
  
  albaVMEGenericAbstract *vme = (albaVMEGenericAbstract *)m_VME;

  if (vme && vme->GetDataVector())
  {
		vtkMTimeType arrayMTime = vme->GetDataVector()->GetMTime();
    if (arrayMTime > mtime)
    {
      return arrayMTime;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
bool albaDataPipeInterpolator::Accept(albaVME *vme)
//------------------------------------------------------------------------------
{
  return vme && vme->IsA(albaVMEGenericAbstract::GetStaticTypeId());
}

//------------------------------------------------------------------------------
void albaDataPipeInterpolator::PreExecute()
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
void albaDataPipeInterpolator::UpdateBounds()
//------------------------------------------------------------------------------
{
  albaVMEItem *old_item=m_CurrentItem;
  this->InternalItemUpdate();

  if (m_CurrentItem)
  { 
    if ( (!m_Bounds.IsValid())||(m_CurrentItem!=old_item)|| \
      (m_CurrentItem->GetMTime()>m_Bounds.GetMTime())|| \
      (m_CurrentItem && m_CurrentItem->GetUpdateTime()>m_Bounds.GetMTime()))
    {
        m_Bounds.DeepCopy(m_CurrentItem->GetBounds());
    }
  }
}

//------------------------------------------------------------------------------
void albaDataPipeInterpolator::InternalItemUpdate()
//------------------------------------------------------------------------------
{  
  albaVMEGenericAbstract *vme=(albaVMEGenericAbstract *)m_VME;
  albaDataVector *array = vme ? vme->GetDataVector() : NULL;

  if (array)
  {
    albaVMEItem *item = array->GetItemBefore(m_CurrentTime);
    UpdateCurrentItem(item);
  }
  
}

//-------------------------------------------------------------------------
void albaDataPipeInterpolator::SetCurrentItem(albaVMEItem *data)
//------------------------------------------------------------------------------
{
  if (data==m_CurrentItem)
    return;
  m_CurrentItem=data;
  Modified();
}

//-------------------------------------------------------------------------
void albaDataPipeInterpolator::UpdateCurrentItem(albaVMEItem *item)
//------------------------------------------------------------------------------
{
  if (item)
  {	
    if (item!=m_CurrentItem)
    {
      if (m_ReleaseDataFlag&&m_CurrentItem)
        m_CurrentItem->ReleaseData();

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
//------------------------------------------------------------------------------
void albaDataPipeInterpolator::OnEvent(albaEventBase *alba_event)
//------------------------------------------------------------------------------
{
  switch (alba_event->GetId())
  {
  case VME_OUTPUT_DATA_PREUPDATE:
    if (GetMTime() > m_PreExecuteTime.GetMTime() || (m_CurrentItem && !m_CurrentItem->IsDataPresent()))
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
