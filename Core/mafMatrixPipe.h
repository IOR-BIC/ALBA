/*========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMatrixPipe.h,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:27:15 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafMatrixPipe_h
#define __mafMatrixPipe_h

#include "mafTransformBase.h"
#include "mafEventSender.h"
#include "mafTimeStamped.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;

/** bridge class between mafTransform pipeline and VME to compute VME output matrix.
  mafMatrixPipe is an functional component representing a bridge between the MAF pipeline
  mechanism for trasnformations (mafTransform) and the VME to produce an output a matrix
  representing the VME pose in space. The matrix pipe send an event to its listener (the VME)
  to rise the call of the PreUpdateMatrix() and UpdateMatrix() callback functions.
  @sa mafMatrixInterpolator mafVME mafMatrixVector mafMatrixInterpolatorHold mafMatrixInterpolatorNearest

  @todo
  - to check the m_Updating effect and usefulness 
  */
class MAF_EXPORT mafMatrixPipe: public mafTransformBase, public mafEventSender
{
public:
  mafTypeMacro(mafMatrixPipe,mafTransformBase);
  
  void UpdateMatrixObserverOn() {m_UpdateMatrixObserverFlag=1;}
  void UpdateMatrixObserverOff() {m_UpdateMatrixObserverFlag=0;}
  void SetUpdateMatrixObserverFlag(bool flag);
  bool GetUpdateMatrixObserverFlag();

  /** set the VME connected to this class. return MAF_ERROR if "vme" is not accepted */
  int SetVME(mafVME *vme);
  /** return the VME this matrix pipe is connected to */
  mafVME *GetVME() {return m_VME;}
    
  /** Set the current time. This makes the pipe to be recomputed. */
  virtual void SetCurrentTime(mafTimeStamp t);

  /** return the time stamp currently set to the pipe */
  mafTimeStamp GetCurrentTime() {return m_CurrentTime;}
  
  /** Get the MTime: this is the bit of magic that makes everything work. */
  virtual unsigned long GetMTime();
  
  /** This function returns true if given VME is accepted by this Pipe. */
  virtual bool Accept(mafVME *vme) {return true;}
  
  /** Redefined to avoid loops while updating */
  virtual const mafMatrix &GetMatrix();
  
  /**
    Make a copy of this pipe, also copying all parameters. This is equivalent to
    NewInstance + DeepCopy
    BEWARE: the returned object has reference counter set to 0. This avoid the 
    to do an extra Delete(), but requires to Register it to ensure keeping it alive.*/
  mafMatrixPipe *MakeACopy();
  
  /**
    Copy from another pipe, the function return MAF_ERROR if the specied pipe 
    is not compatible. */
  virtual int DeepCopy(mafMatrixPipe *pipe);
  
protected:
  mafMatrixPipe();
  virtual ~mafMatrixPipe();

  /** To be redefined by subclasses to override Pipe behavior */
  virtual void InternalUpdate();

  bool          m_UpdateMatrixObserverFlag;
  mafTimeStamp  m_CurrentTime;
  bool          m_Updating;
  mafVME        *m_VME; ///< pointer to VME

private:
  mafMatrixPipe(const mafMatrixPipe&); // Not implemented
  void operator=(const mafMatrixPipe&); // Not implemented
  
};

#endif /* __mafMatrixPipe_h */
 
