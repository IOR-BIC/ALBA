/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaMeter.h,v $
  Language:  C++
  Date:      $Date: 2005-06-10 08:54:43 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone, Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmaMeter_h
#define __mmaMeter_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAttribute.h"

/** mmaMeter 
*/
class MAF_EXPORT mmaMeter : public mafAttribute
{
public:
  mmaMeter();
  virtual ~mmaMeter();

  mafTypeMacro(mmaMeter, mafAttribute);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const mafAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const mafAttribute *a);

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
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};
#endif