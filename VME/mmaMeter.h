/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaMeter
 Authors: Marco Petrone, Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mmaMeter_h
#define __mmaMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaAttribute.h"

/** mmaMeter 
*/
class ALBA_EXPORT mmaMeter : public albaAttribute
{
public:
  mmaMeter();
  virtual ~mmaMeter();

  albaTypeMacro(mmaMeter, albaAttribute);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const albaAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const albaAttribute *a);

  int m_MeterMode;
  int m_ColorMode;
  int m_Representation;
  int m_Capping;
  int m_MeasureType;

  int m_GenerateEvent;
  int m_ThresholdEvent;

  double m_InitMeasure;
  double m_DeltaPercent;
  double m_TubeRadius;
  double m_DistanceRange[2];

  int    m_LabelVisibility;

protected:
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);
};
#endif