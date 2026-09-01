/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixPipe
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaMatrixPipe_h
#define __albaMatrixPipe_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaTransformBase.h"
#include "albaTimeStamped.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;

/** bridge class between albaTransform pipeline and VME to compute VME output matrix.
  albaMatrixPipe is an functional component representing a bridge between the ALBA pipeline
  mechanism for trasnformations (albaTransform) and the VME to produce an output a matrix
  representing the VME pose in space. The matrix pipe send an event to its listener (the VME)
  to rise the call of the PreUpdateMatrix() and UpdateMatrix() callback functions.
  @sa albaMatrixInterpolator albaVME albaMatrixVector albaMatrixInterpolatorHold albaMatrixInterpolatorNearest

  @todo
  - to check the m_Updating effect and usefulness 
  */
class ALBA_EXPORT albaMatrixPipe: public albaTransformBase
{
public:
  albaMatrixPipe();
  virtual ~albaMatrixPipe();

  albaTypeMacro(albaMatrixPipe,albaTransformBase);
  
  void UpdateMatrixObserverOn() {m_UpdateMatrixObserverFlag=1;}
  void UpdateMatrixObserverOff() {m_UpdateMatrixObserverFlag=0;}
  void SetUpdateMatrixObserverFlag(bool flag) {m_UpdateMatrixObserverFlag=flag;}
  bool GetUpdateMatrixObserverFlag() {return m_UpdateMatrixObserverFlag;}

  /** set the VME connected to this class. return ALBA_ERROR if "vme" is not accepted */
  int SetVME(albaVME *vme);
  /** return the VME this matrix pipe is connected to */
  albaVME *GetVME() {return m_VME;}
    
  /** Set the current time. This makes the pipe to be recomputed. */
  virtual void SetTimeStamp(albaTimeStamp t);

  /** return the time stamp currently set to the pipe */
  albaTimeStamp GetTimeStamp();
  
  /** Get the MTime: this is the bit of magic that makes everything work. */
  virtual vtkMTimeType GetMTime();
  
  /** This function returns true if given VME is accepted by this Pipe. */
  virtual bool Accept(albaVME *vme) {return true;}
  
  /** Redefined to avoid loops while updating */
  virtual const albaMatrix &GetMatrix();
  
  /**
    Make a copy of this pipe, also copying all parameters. This is equivalent to
    NewInstance + DeepCopy
    BEWARE: the returned object has reference counter set to 0. This avoid the 
    to do an extra Delete(), but requires to Register it to ensure keeping it alive.*/
  albaMatrixPipe *MakeACopy();
  
  /**
    Copy from another pipe, the function return ALBA_ERROR if the specied pipe 
    is not compatible. */
  virtual int DeepCopy(albaMatrixPipe *pipe);

  /** 
    Redefined to send pre update event to the VME. In the pre update event
    the VME can change pipe parameters */
  virtual void Update();
  
protected:
  /** To be redefined by subclasses to override Pipe behavior */
  virtual void InternalUpdate();

  bool          m_UpdateMatrixObserverFlag;
  bool          m_Updating;
  albaVME        *m_VME; ///< pointer to VME

private:
  albaMatrixPipe(const albaMatrixPipe&); // Not implemented
  void operator=(const albaMatrixPipe&); // Not implemented
  
};

#endif /* __albaMatrixPipe_h */
 
