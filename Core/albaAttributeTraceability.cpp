/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAttributeTraceability
 Authors: Roberto Mucci
 
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

#include "albaAttributeTraceability.h"
#include "albaStorageElement.h"
#include "albaIndent.h"

#include <ostream>

albaCxxTypeMacro(albaAttributeTraceability);

//----------------------------------------------------------------------------
albaAttributeTraceability::albaAttributeTraceability()
//----------------------------------------------------------------------------
{  
  m_TraceabilityVector.clear();
}
//----------------------------------------------------------------------------
albaAttributeTraceability::~albaAttributeTraceability()
//----------------------------------------------------------------------------
{
  m_TraceabilityVector.clear();
}

//----------------------------------------------------------------------------
void albaAttributeTraceability::AddTraceabilityEvent(albaString trialEvent, albaString operationName, albaString parmaters, albaString date, albaString appStamp, albaString operatorID, albaString isNatural)
//----------------------------------------------------------------------------
{  
  m_Traceability traceability;
  traceability.m_TrialEvent = trialEvent;
  traceability.m_OperationName = operationName;
  traceability.m_Parameters = parmaters;
  traceability.m_Date = date;
  traceability.m_AppStamp = appStamp;
  traceability.m_OperatorID = operatorID;
  traceability.m_IsNatural = isNatural;
  m_TraceabilityVector.push_back(traceability);
}

//----------------------------------------------------------------------------
void albaAttributeTraceability::RemoveTraceabilityEvent()
//----------------------------------------------------------------------------
{  
  if (m_TraceabilityVector.size() != 0)
  {
    m_TraceabilityVector.pop_back();
  }
}

//-------------------------------------------------------------------------
void albaAttributeTraceability::DeepCopy(const albaAttribute *a)
//-------------------------------------------------------------------------
{
  Superclass::DeepCopy(a);
  int vecSize = ((albaAttributeTraceability *)a)->m_TraceabilityVector.size();
  for (int i = 0; i < vecSize; i++)
  {
    m_Traceability traceability;
    traceability.m_TrialEvent = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent;
    traceability.m_OperationName = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperationName;
    traceability.m_Parameters = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_Parameters;
    traceability.m_Date = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_Date;
    traceability.m_AppStamp = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_AppStamp;
    traceability.m_OperatorID = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperatorID;
    if (((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent == "Create")
    {
      traceability.m_IsNatural = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_IsNatural;
    }
    m_TraceabilityVector.push_back(traceability);
  }
}

//-------------------------------------------------------------------------
bool albaAttributeTraceability::Equals(const albaAttribute *a) const
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    int vecSize = ((albaAttributeTraceability *)a)->m_TraceabilityVector.size();
    for (int i = 0; i < vecSize; i++)
    {
      if(m_TraceabilityVector[i].m_TrialEvent != ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent ||
        m_TraceabilityVector[i].m_OperationName != ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperationName ||
        m_TraceabilityVector[i].m_Parameters != ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_Parameters ||
        m_TraceabilityVector[i].m_Date != ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_Date ||
        m_TraceabilityVector[i].m_AppStamp != ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_AppStamp ||
        m_TraceabilityVector[i].m_OperatorID != ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperatorID)
        return false;

      if (((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent == "Create")
      {
        if (m_TraceabilityVector[i].m_IsNatural != ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_IsNatural)
          return false;
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------------
int albaAttributeTraceability::InternalStore(albaStorageElement *parent)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    for (int i = 0; i < m_TraceabilityVector.size(); i++)
    {
      parent->StoreText("TrialEvent", m_TraceabilityVector[i].m_TrialEvent.GetCStr());
      parent->StoreText("Operation", m_TraceabilityVector[i].m_OperationName.GetCStr());
      parent->StoreText("Parameters", m_TraceabilityVector[i].m_Parameters.GetCStr());
      parent->StoreText("Date", m_TraceabilityVector[i].m_Date.GetCStr());
      parent->StoreText("Application", m_TraceabilityVector[i].m_AppStamp.GetCStr());
      parent->StoreText("OperatorID",  m_TraceabilityVector[i].m_OperatorID.GetCStr());
      if (m_TraceabilityVector[i].m_TrialEvent == "Create")
      {
        parent->StoreText("IsNatural", m_TraceabilityVector[i].m_IsNatural.GetCStr());
      }
    }  
  }
  return ALBA_OK;
}

//-------------------------------------------------------------------------
int albaAttributeTraceability::InternalRestore(albaStorageElement *node)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == ALBA_OK)
  {
    m_Traceability traceability;

    std::vector<albaStorageElement*> listTrialEvent;
    node->GetNestedElementsByName("TrialEvent", listTrialEvent);

    std::vector<albaStorageElement*> listOperation;
    node->GetNestedElementsByName("Operation", listOperation);

    std::vector<albaStorageElement*> listParameters;
    node->GetNestedElementsByName("Parameters", listParameters);

    std::vector<albaStorageElement*> listDate;
    node->GetNestedElementsByName("Date", listDate);

    std::vector<albaStorageElement*> listApplication;
    node->GetNestedElementsByName("Application", listApplication);

    std::vector<albaStorageElement*> listOperatorID;
    node->GetNestedElementsByName("OperatorID", listOperatorID);

    std::vector<albaStorageElement*> listIsNatural;
    node->GetNestedElementsByName("IsNatural", listIsNatural);

    try
    {
      int iCreateIdx = 0;   //BES: 27.11.2008 - BUG FIX
      for (int i  = 0; i < listTrialEvent.size(); i++)
      {
        m_Traceability traceability;

        listTrialEvent[i]->RestoreText(traceability.m_TrialEvent);
        listOperation[i]->RestoreText(traceability.m_OperationName);
        if (listParameters.size() > i)
          listParameters[i]->RestoreText(traceability.m_Parameters);
        listDate[i]->RestoreText(traceability.m_Date);
        listApplication[i]->RestoreText(traceability.m_AppStamp);
        listOperatorID[i]->RestoreText(traceability.m_OperatorID);
        if (traceability.m_TrialEvent == "Create")
        {
          //BES: 27.11.2008 - BUG FIX - listIsNatural can contain less items than listTrialEvent
          listIsNatural[iCreateIdx++]->RestoreText(traceability.m_IsNatural);  
        }
        m_TraceabilityVector.push_back(traceability);
      }
    }
    catch (...)
    {
      albaLogMessage("Problems restoring audit trials attribute");
    }
    
   
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
albaString albaAttributeTraceability::GetLastTrialEvent()
//-------------------------------------------------------------------------
{
  albaString trialType;
  if (m_TraceabilityVector.size() != 0)
  {
    trialType = m_TraceabilityVector[m_TraceabilityVector.size()-1].m_TrialEvent;
  }
  return trialType;
}
