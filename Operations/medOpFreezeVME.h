/*=========================================================================

 Program: MAF2
 Module: medOpFreezeVME
 Authors: Daniele Giunchi , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpFreezeVME_H__
#define __medOpFreezeVME_H__

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
// medOpFreezeVME :
//----------------------------------------------------------------------------
/** This operation coverts a procedural vme in the corresponding version not procedural*/
class MAF_EXPORT medOpFreezeVME: public mafOp
{
public:
	medOpFreezeVME(wxString label = "Freeze VME");
	~medOpFreezeVME(); 
	
  mafTypeMacro(medOpFreezeVME, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	
  /** Builds operation's interface. */
	void OpRun();
	void OpDo();

protected:


};
#endif
