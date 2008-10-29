/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAttributeTraceability.h,v $
  Language:  C++
  Date:      $Date: 2008-10-29 11:03:21 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafAttributeTraceability_h
#define __mafAttributeTraceability_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafAttribute.h"
#include <vector>

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

  std::vector<m_Traceability> m_TraceabilityVector;

protected:
  
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};

#endif 

