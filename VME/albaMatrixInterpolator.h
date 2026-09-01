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
#ifndef __albaMatrixInterpolator_h
#define __albaMatrixInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaMatrixPipe.h"
#include "albaSmartPointer.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaMatrix;

/** Class Name: albaMatrixInterpolator.
  albaMatrixInterpolator is used for computing the position of VME in terms 
  of matrix that is an element of vector of items.
  albaMatrixInterpolator is a matrix pipe producing as output a matrix interpolated
  against the ones stored in the array associated to the VME itself (only for VMEs
  with matrix array like albaVMEGenericAbstract). 

*/
class ALBA_EXPORT albaMatrixInterpolator:public albaMatrixPipe
{
public:
  /** type macro for RTTI and instance creation.*/
  albaTypeMacro(albaMatrixInterpolator,albaMatrixPipe);

  /** This matrix pipe accepts only VME's with internal matrix Array. */
  virtual bool Accept(albaVME *vme);

  /**  Get the output of the interpolator item. */
  albaMatrix *GetCurrentItem() {return m_CurrentItem;}

  /**
  Set the current time. Overidden to allow the output not to change when 
  not necessary, e.g. constant pose (in time) being interpolated should not
  produce a change in the output data when time is changed.*/
  virtual void SetTimeStamp(albaTimeStamp time);

  /**
  Get the MTime: this is the bit of magic that makes everything work.*/
  virtual vtkMTimeType GetMTime();

  /** Force update of the pipe.*/
  virtual void Update();

protected:
  /** constructor.*/
  albaMatrixInterpolator();
  /** destructor.*/
  virtual ~albaMatrixInterpolator();

  //virtual void PreExecute(){};
  //virtual void Execute(){};

  /**
  This function is called internally to updated the output data according
  interpolator rules. It should be reimplemented in sub-classes.*/
  virtual void InternalItemUpdate();

  /** update the output matrix. */
  virtual void InternalUpdate();

  /** Set the current matrix of the pipe. */
  void SetCurrentItem(albaMatrix *data);

  /** request the update of the current matrix of the pipe. */
  void UpdateCurrentItem(albaMatrix *item);

  albaAutoPointer<albaMatrix>    m_CurrentItem; ///< the item currently selected for the current time

  albaAutoPointer<albaMatrix>    m_OldItem;     ///< the item previously selected

  albaTimeStamp  m_OldTimeStamp; ///< previous time
private:
  /** copy constructor not implemented. */
  albaMatrixInterpolator(const albaMatrixInterpolator&);
  /** assignment operator not implemeted. */
  void operator=(const albaMatrixInterpolator&); // Not implemented
  
};

#endif /* __albaMatrixInterpolator_h */
 
