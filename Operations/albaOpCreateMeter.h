/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateMeter
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateMeter_H__
#define __albaOpCreateMeter_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEMeter;
class albaGUI;
class albaEvent;
//----------------------------------------------------------------------------
// albaOpCreateMeter :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpCreateMeter: public albaOp
{
public:
  albaOpCreateMeter(const wxString &label = "CreateMeter");
  ~albaOpCreateMeter(); 

  albaTypeMacro(albaOpCreateMeter, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

  albaVMEMeter *m_Meter;
};
#endif
