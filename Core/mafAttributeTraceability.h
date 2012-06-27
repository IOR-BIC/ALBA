/*=========================================================================

 Program: MAF2
 Module: mafAttributeTraceability
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafAttributeTraceability_h
#define __mafAttributeTraceability_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafAttribute.h"
#include <vector>

struct m_Traceability
{
  mafString m_TrialEvent;
  mafString m_OperationName;
  mafString m_Parameters;
  mafString m_Date;
  mafString m_AppStamp;
  mafString m_OperatorID;
  mafString m_IsNatural;
};

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,m_Traceability);
#endif

//----------------------------------------------------------------------------
// mafAttributeTraceability:
//----------------------------------------------------------------------------
/** Set trials information about operations which created or modified VME */  
class MAF_EXPORT mafAttributeTraceability : public mafAttribute
{
public:
           mafAttributeTraceability();
  virtual ~mafAttributeTraceability();
  
  mafTypeMacro(mafAttributeTraceability,mafAttribute);

  /** add a create event */
  void AddTraceabilityEvent(mafString m_TrialEvent, mafString operationName, mafString parmaters, mafString date, mafString appStamp, mafString operatorID, mafString isNatural = "");
 
  /** remove a traceability event */
  void RemoveTraceabilityEvent(); 

  /** copy the content of the given attribute. Attributes must be type compatible */
  virtual void DeepCopy(const mafAttribute *a);

  /** create a copy of this attribute */
  mafAttributeTraceability *MakeCopy();

  /** return true if this attribute equals the given one */
  bool Equals(const mafAttribute *a) const;

  /** return trial type form vetor of traceability event*/
  mafString GetLastTrialEvent();

  std::vector<m_Traceability> m_TraceabilityVector;

protected:
  
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};

#endif 

