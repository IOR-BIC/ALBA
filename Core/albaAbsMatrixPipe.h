/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAbsMatrixPipe
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaAbsMatrixPipe_h
#define __albaAbsMatrixPipe_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaMatrixPipe.h"
#include "albaMatrix.h"
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class albaTransformFrame;

/** albaAbsMatrixPipe - this class implements VME tree direct cinematic (AbsMatrix).
  albaAbsMatrixPipe is a MatrixPipe implementing the VME tree direct cinematic of 
  absolute matrices. It supports efficient on demand update of the output matrix for the
  current time being coincident with the VME's one. Also it supports computing of the abs-matrix
  for arbitrary time with a slightly less efficient mechanism, by temporary switching current time of 
  the pointed VME and its parent to a diffent time. In the later case (i.e. when this class CurrentTime
  differs from VME current time) no MatriUpdateEvent is rised! 
 
  @sa albaMatrixPipe albaVME albaMatrixVector

  @todo
  - check if it's possible to remove the m_Updating flag!
*/ 
class ALBA_EXPORT albaAbsMatrixPipe: public albaMatrixPipe
{
public:
  albaAbsMatrixPipe();
  virtual ~albaAbsMatrixPipe();
  
  albaTypeMacro(albaAbsMatrixPipe,albaMatrixPipe);

  /** Set the VME to be used as input for this PIPE */
  virtual int SetVME(albaVME *vme);

  /** 
    Overridden to take into consideration the internal transform
    object MTime. */
  virtual vtkMTimeType GetMTime();
  
protected:
  
  virtual void InternalUpdate();

  albaTransformFrame *m_Transform; ///< internal transform used to compute the local to ABS frame transformation

private:
  albaAbsMatrixPipe(const albaAbsMatrixPipe&); // Not implemented
  void operator=(const albaAbsMatrixPipe&); // Not implemented
  
};

#endif /* __albaAbsMatrixPipe_h */
 
