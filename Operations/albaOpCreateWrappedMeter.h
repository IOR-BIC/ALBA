/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateWrappedMeter
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateWrappedMeter_H__
#define __albaOpCreateWrappedMeter_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEWrappedMeter;
class albaGui;
class albaEvent;

/** 
  class name: albaOpCreateWrappedMeter
  Operation that create a albaVMEWrappedMeter for wrap muscle action lines.
*/
class ALBA_EXPORT albaOpCreateWrappedMeter: public albaOp
{
public:
  /** constructor */
  albaOpCreateWrappedMeter(const wxString &label = "Create Wrapped Meter");
  /** destructor */
  ~albaOpCreateWrappedMeter(); 

  /** RTTI macro*/
  albaTypeMacro(albaOpCreateWrappedMeter, albaOp);

  /** clone the object and retrieve a copy*/
  albaOp* Copy();

  /** Builds operation's interface. */
  void OpRun();
  /** Execute the operation. */
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  albaVMEWrappedMeter *m_Meter;
};
#endif
