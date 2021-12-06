/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateSurface_H__
#define __albaOpCreateSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMESurface;

//----------------------------------------------------------------------------
// albaOpCreateSurface :
//----------------------------------------------------------------------------
/**Convert a Parametric Surface into a Normal Surface*/
class ALBA_EXPORT albaOpCreateSurface: public albaOp
{
public:

	albaOpCreateSurface(wxString label = "Create Surface");
	~albaOpCreateSurface(); 

	albaTypeMacro(albaOpCreateSurface, albaOp);

	albaOp* Copy();

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun();

	/** Execute the operation. */
	virtual void OpDo();

protected:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

	albaVMESurface *m_Surface;

};
#endif
