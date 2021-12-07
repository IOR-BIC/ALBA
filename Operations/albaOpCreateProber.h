/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateProber
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateProber_H__
#define __albaOpCreateProber_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEProber;
class albaGUI;
class albaEvent;
//----------------------------------------------------------------------------
// albaOpCreateProber :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpCreateProber: public albaOp
{
public:
  albaOpCreateProber(const wxString &label = "CreateProber");
 ~albaOpCreateProber(); 

  albaTypeMacro(albaOpCreateProber, albaOp);

  albaOp* Copy();

	void OpRun();
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

  albaVMEProber *m_Prober;
};
#endif
