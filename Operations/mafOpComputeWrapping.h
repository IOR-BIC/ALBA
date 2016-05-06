/*=========================================================================

 Program: MAF2
 Module: mafOpComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpComputeWrapping_H__
#define __mafOpComputeWrapping_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medVMEComputeWrapping;
class mafGui;
class mafEvent;

/** 
  class name: mafOpComputeWrapping
  Operation used to create a medVMEComputeWrapping used to wrap muscle action lines.
*/
class MAF_EXPORT mafOpComputeWrapping: public mafOp
{
public:
  /** constructor */
  mafOpComputeWrapping(const wxString &label = "CreateWrappedMeter");
  /** destructor */
  ~mafOpComputeWrapping();

  /** RTTI macro*/
  mafTypeMacro(mafOpComputeWrapping, mafOp);

  /** clone the object and retrieve a copy*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);
  /** Builds operation's interface. */
  void OpRun();
  /** Execute the operation. */
  void OpDo();

protected: 
  medVMEComputeWrapping *m_Meter;
};
#endif
