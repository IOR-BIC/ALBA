/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAbsMatrixPipe.h,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:27:15 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAbsMatrixPipe_h
#define __mafAbsMatrixPipe_h

#include "mafMatrixPipe.h"
#include "mafMatrix.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafTransformFrame;

/** mafAbsMatrixPipe - this class implements VME tree direct cinematic (AbsMatrix).
  mafAbsMatrixPipe is a MatrixPipe implementing the VME tree direct cinematic of 
  absolute matrices. It supports efficient on demand update of the output matrix for the
  current time being coincident with the VME's one. Also it supports computing of the abs-matrix
  for arbitrary time with a slightly less efficient mechanism, by temporary switching current time of 
  the pointed VME and its parent to a diffent time. In the later case (i.e. when this class CurrentTime
  differs from VME current time) no MatriUpdateEvent is rised! 
 
  @sa mafMatrixPipe mafVME mafMatrixVector

  @todo
  - check if it's possible to remove the m_Updating flag!
*/ 
class MAF_EXPORT mafAbsMatrixPipe: public mafMatrixPipe
{
public:
  mafAbsMatrixPipe();
  virtual ~mafAbsMatrixPipe();
  
  mafTypeMacro(mafAbsMatrixPipe,mafMatrixPipe);

  /** Set the VME to be used as input for this PIPE */
  virtual int SetVME(mafVME *vme);

  /** 
    Overridden to take into consideration the internal transform
    object MTime. */
  virtual unsigned long GetMTime();
  
protected:
  
  virtual void InternalUpdate();

  mafTransformFrame *m_Transform; ///< internal transform used to compute the local to ABS frame transformation

private:
  mafAbsMatrixPipe(const mafAbsMatrixPipe&); // Not implemented
  void operator=(const mafAbsMatrixPipe&); // Not implemented
  
};

#endif /* __mafAbsMatrixPipe_h */
 
