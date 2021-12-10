/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpComputeWrapping
 Authors: Anupam Agrawal and Hui Wei
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpComputeWrapping_H__
#define __albaOpComputeWrapping_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEComputeWrapping;
class albaGui;
class albaEvent;

/** 
  class name: albaOpComputeWrapping
  Operation used to create a medVMEComputeWrapping used to wrap muscle action lines.
*/
class ALBA_EXPORT albaOpComputeWrapping: public albaOp
{
public:
  /** constructor */
  albaOpComputeWrapping(const wxString &label = "CreateWrappedMeter");
  /** destructor */
  ~albaOpComputeWrapping();

  /** RTTI macro*/
  albaTypeMacro(albaOpComputeWrapping, albaOp);

  /** clone the object and retrieve a copy*/
  albaOp* Copy();

  /** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  albaVMEComputeWrapping *m_Meter;
};
#endif
