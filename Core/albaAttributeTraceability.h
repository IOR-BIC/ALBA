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
#ifndef __albaAttributeTraceability_h
#define __albaAttributeTraceability_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaAttribute.h"
#include <vector>

struct albaTraceability
{
  albaString m_TrialEvent;
  albaString m_OperationName;
  albaString m_Parameters;
  albaString m_Date;
  albaString m_AppStamp;
	albaString m_BuildNum;
  albaString m_OperatorID;
  albaString m_IsNatural;
};

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaTraceability);
#endif

//----------------------------------------------------------------------------
// albaAttributeTraceability:
//----------------------------------------------------------------------------
/** Set trials information about operations which created or modified VME */  
class ALBA_EXPORT albaAttributeTraceability : public albaAttribute
{
public:
           albaAttributeTraceability();
  virtual ~albaAttributeTraceability();
  
  albaTypeMacro(albaAttributeTraceability,albaAttribute);

  /** add a create event */
  void AddTraceabilityEvent(albaString m_TrialEvent, albaString operationName, albaString parmaters, albaString date, albaString appStamp,albaString buildNum, albaString operatorID, albaString isNatural);
 
  /** remove a traceability event */
  void RemoveTraceabilityEvent(); 

  /** copy the content of the given attribute. Attributes must be type compatible */
  virtual void DeepCopy(const albaAttribute *a);

  /** create a copy of this attribute */
  albaAttributeTraceability *MakeCopy();

  /** return true if this attribute equals the given one */
  bool Equals(const albaAttribute *a) const;

  /** return trial type form vetor of traceability event*/
  albaString GetLastTrialEvent();

  std::vector<albaTraceability> m_TraceabilityVector;

protected:
  
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);
};

#endif 

