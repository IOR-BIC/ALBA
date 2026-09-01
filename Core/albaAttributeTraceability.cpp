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
void albaAttributeTraceability::AddTraceabilityEvent(albaString trialEvent, albaString operationName, albaString parmaters, albaString date, albaString appStamp,albaString buildNum, albaString operatorID, albaString isNatural)
//----------------------------------------------------------------------------
{  
  albaTraceability traceability;
  traceability.m_TrialEvent = trialEvent;
  traceability.m_OperationName = operationName;
  traceability.m_Parameters = parmaters;
  traceability.m_Date = date;
  traceability.m_AppStamp = appStamp;
	traceability.m_BuildNum = buildNum;
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
    albaTraceability traceability;
    traceability.m_TrialEvent = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_TrialEvent;
    traceability.m_OperationName = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_OperationName;
    traceability.m_Parameters = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_Parameters;
    traceability.m_Date = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_Date;
		traceability.m_AppStamp = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_AppStamp;
		traceability.m_BuildNum = ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_BuildNum;
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
				m_TraceabilityVector[i].m_BuildNum != ((albaAttributeTraceability *)a)->m_TraceabilityVector[i].m_BuildNum ||
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
			parent->StoreText("BuildNum", m_TraceabilityVector[i].m_BuildNum.GetCStr());
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
    albaTraceability traceability;
		
		albaString nameStr = "Name";
		albaString trialEventStr = "TrialEvent";
		albaString operationStr = "Operation";
		albaString parametersStr = "Parameters";
		albaString dateStr = "Date";
		albaString applicationStr = "Application";
		albaString buildNumStr = "BuildNum";
		albaString operatorIDStr = "OperatorID";
		albaString isNaturalStr = "IsNatural";

		std::vector<albaStorageElement*> children = node->GetChildren();

		int pos = 0;
    while (pos<children.size())
    {
      albaTraceability traceability;

			if (pos == 0 && nameStr == children[pos]->GetName())
				pos++;

			//Trial Event
			if (trialEventStr != children[pos]->GetName())
				return ALBA_ERROR;
		  children[pos]->RestoreText(traceability.m_TrialEvent);
			pos++;
			if (pos >= children.size())
				return ALBA_ERROR;

			//Op Name
			if (operationStr != children[pos]->GetName())
				return ALBA_ERROR;
			children[pos]->RestoreText(traceability.m_OperationName);
				pos++;
			if (pos >= children.size())
				return ALBA_ERROR;

			//Parameters
			if (parametersStr != children[pos]->GetName())
				return ALBA_ERROR;
			children[pos]->RestoreText(traceability.m_Parameters);
			pos++;
			if (pos >= children.size())
				return ALBA_ERROR;

			//Date
			if (dateStr != children[pos]->GetName())
				return ALBA_ERROR;
			children[pos]->RestoreText(traceability.m_Date);
			pos++;
			if (pos >= children.size())
				return ALBA_ERROR;

			//App Stamp
			if (applicationStr != children[pos]->GetName())
				return ALBA_ERROR;
			children[pos]->RestoreText(traceability.m_AppStamp);
			pos++;
			if (pos >= children.size())
				return ALBA_ERROR;

			//BuildNum is not mandatory
			if (buildNumStr == children[pos]->GetName())
			{
				children[pos]->RestoreText(traceability.m_BuildNum);
				pos++;
				if (pos >= children.size())
					return ALBA_ERROR;
			}

			//Operator Id can be the last item
			if (operatorIDStr != children[pos]->GetName())
				return ALBA_ERROR;
			children[pos]->RestoreText(traceability.m_OperatorID);
			pos++;
			
		  if (traceability.m_TrialEvent == "Create")
      {
				//Check pos from operator id only if there is the isNatural Attribute
				if (pos >= children.size())
					return ALBA_ERROR;

				if (isNaturalStr != children[pos]->GetName())
					return ALBA_ERROR;
				children[pos]->RestoreText(traceability.m_IsNatural);
				pos++;
		  }
      m_TraceabilityVector.push_back(traceability);
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
