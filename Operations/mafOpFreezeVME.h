/*=========================================================================

 Program: MAF2
 Module: mafOpFreezeVME
 Authors: Daniele Giunchi , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpFreezeVME_H__
#define __mafOpFreezeVME_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// mafOpFreezeVME :
//----------------------------------------------------------------------------
/** This operation coverts a procedural vme in the corresponding version not procedural*/
class MAF_EXPORT mafOpFreezeVME: public mafOp
{
public:
	mafOpFreezeVME(wxString label = "Freeze VME");
	~mafOpFreezeVME(); 
	
  mafTypeMacro(mafOpFreezeVME, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME*node);

	
  /** Builds operation's interface. */
	void OpRun();
	void OpDo();

protected:


};
#endif
