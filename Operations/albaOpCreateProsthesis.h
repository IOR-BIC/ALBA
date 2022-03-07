/*=========================================================================
Program:   AssemblerPro
Module:    albaOpCreateProsthesis.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpCreateProsthesis_H__
#define __albaOpCreateProsthesis_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaVMEProsthesis;
//----------------------------------------------------------------------------
// Class Name: albaOpCreateProsthesis
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpCreateProsthesis : public albaOp
{
public:
	/** Constructor. */
	albaOpCreateProsthesis(wxString label = "Create Prosthesis");

	/** Destructor. */
	~albaOpCreateProsthesis();

	/** RTTI macro. */
	albaTypeMacro(albaOpCreateProsthesis, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();


	albaVMEProsthesis * CreateVMEProshesis();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	albaProsthesesDBManager * m_ProsthesesDBManager;
};

#endif
