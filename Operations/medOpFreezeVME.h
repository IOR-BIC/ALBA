/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpFreezeVME.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:08:41 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpFreezeVME_H__
#define __medOpFreezeVME_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// medOpFreezeVME :
//----------------------------------------------------------------------------
/** This operation coverts a procedural vme in the corresponding version not procedural*/
class MED_EXPORT medOpFreezeVME: public mafOp
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
