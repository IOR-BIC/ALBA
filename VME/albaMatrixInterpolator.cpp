/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixInterpolator
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


#include "albaMatrixInterpolator.h"

#include "albaVME.h"
#include "albaMatrix.h"
#include "albaVMEGenericAbstract.h"

#include "albaMatrixVector.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaMatrixInterpolator)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaMatrixInterpolator::albaMatrixInterpolator()
//------------------------------------------------------------------------------
{
  m_CurrentItem   = NULL;
  m_OldTimeStamp  = -1;
  m_OldItem       = NULL;
}

//------------------------------------------------------------------------------
albaMatrixInterpolator::~albaMatrixInterpolator()
//------------------------------------------------------------------------------
{
  SetCurrentItem(NULL);
}

//----------------------------------------------------------------------------
void albaMatrixInterpolator::SetTimeStamp(albaTimeStamp time)
//----------------------------------------------------------------------------
{
  albaTransformBase::SetTimeStamp(time);
  
  // Do not call Modified(), to avoid the pipeline
  // to be automatically updated
}

//----------------------------------------------------------------------------
// Get the MTime. Take in consideration also modifications to the Input Array
vtkMTimeType albaMatrixInterpolator::GetMTime()
//------------------------------------------------------------------------------
{
	vtkMTimeType mtime = Superclass::GetMTime();
  
  albaVMEGenericAbstract *vme = (albaVMEGenericAbstract *)m_VME;

  if (vme && vme->GetMatrixVector())
  {
		vtkMTimeType arrayMTime = vme->GetMatrixVector()->GetMTime();
    if (arrayMTime > mtime)
    {
      return arrayMTime;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------------
bool albaMatrixInterpolator::Accept(albaVME *vme)
//------------------------------------------------------------------------------
{
  return vme && vme->IsA(albaVMEGenericAbstract::GetStaticTypeId());
}

//------------------------------------------------------------------------------
void albaMatrixInterpolator::Update()
//------------------------------------------------------------------------------
{

  unsigned long mtime=m_UpdateTime.GetMTime();

  // if the current time has changed or if this object has been modified...
  if (m_OldTimeStamp!=GetTimeStamp() || !m_CurrentItem || (m_CurrentItem->GetMTime()>mtime) || mtime < GetMTime())
  {
    m_OldTimeStamp=GetTimeStamp();    
    
    // First of all find the right item to be used as input
    // and update the output bounds
    InternalItemUpdate();
  }
  
  Superclass::Update();
}

//------------------------------------------------------------------------------
void albaMatrixInterpolator::InternalItemUpdate()
//------------------------------------------------------------------------------
{  
  albaVMEGenericAbstract *vme=(albaVMEGenericAbstract *)m_VME;
  albaMatrixVector *array = vme ? vme->GetMatrixVector() : NULL;

  if (array)
  {
    albaMatrix *item = array->GetItemBefore(GetTimeStamp());
    UpdateCurrentItem(item);
  }
  
}

//-------------------------------------------------------------------------
void albaMatrixInterpolator::SetCurrentItem(albaMatrix *data)
//------------------------------------------------------------------------------
{
  if (data==m_CurrentItem.GetPointer())
    return;

  m_CurrentItem=data;
  Modified(); // change modification time to force internal update
}

//-------------------------------------------------------------------------
void albaMatrixInterpolator::UpdateCurrentItem(albaMatrix *item)
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
void albaMatrixInterpolator::InternalUpdate()
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
