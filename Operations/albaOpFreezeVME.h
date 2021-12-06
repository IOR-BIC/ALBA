/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFreezeVME
 Authors: Daniele Giunchi , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpFreezeVME_H__
#define __albaOpFreezeVME_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// albaOpFreezeVME :
//----------------------------------------------------------------------------
/** This operation coverts a procedural vme in the corresponding version not procedural*/
class ALBA_EXPORT albaOpFreezeVME: public albaOp
{
public:
	albaOpFreezeVME(wxString label = "Freeze VME");
	~albaOpFreezeVME(); 
	
  albaTypeMacro(albaOpFreezeVME, albaOp);

  albaOp* Copy();
	
  /** Builds operation's interface. */
	void OpRun();
	void OpDo();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);
};
#endif
