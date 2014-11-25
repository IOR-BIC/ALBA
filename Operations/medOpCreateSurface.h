/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateSurface
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpCreateSurface_H__
#define __medOpCreateSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMESurface;

//----------------------------------------------------------------------------
// medOpCreateSurface :
//----------------------------------------------------------------------------
/**Convert a Parametric Surface into a Normal Surface*/
class MAF_EXPORT medOpCreateSurface: public mafOp
{
public:

	medOpCreateSurface(wxString label = "Create Surface");
	~medOpCreateSurface(); 

	mafTypeMacro(medOpCreateSurface, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme);

	/** Builds operation's interface by calling CreateOpDialog() method. */
	void OpRun();

	/** Execute the operation. */
	virtual void OpDo();

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	virtual void OpStop(int result);

	mafVMESurface *m_Surface;

};
#endif
