/*=========================================================================
Program:   Bonemat
Module:    albaOpLoadScalarFromDicFile.h
Language:  C++
Date:      $Date: 2010-11-23 16:50:26 $
Version:   $Revision: 1.1.1.1.2.3 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpImporterDicFile_H__
#define __albaOpImporterDicFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaTextFileReaderHelper.h"
#include "albaDefines.h"


//----------------------------------------------------------------------------
// albaOpLoadScalarFromDicFile :
//----------------------------------------------------------------------------
/** 
Reads scalars from a .DIC file
*/
class ALBA_EXPORT albaOpImporterDicFile : public albaTextFileReaderHelper, public albaOp
{
public:

	albaOpImporterDicFile(const wxString &label = "Load DIC file");
	~albaOpImporterDicFile(); 

	albaTypeMacro(albaOpImporterDicFile, albaOp);

	/** Builds operation's interface. */
	void OpRun();

	albaOp* Copy();

	/** Get the filename to import */
	albaString GetFileName() const { return m_FileName; }

	/** Get the filename to import */
	void SetFileName(albaString val) { m_FileName = val; }

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** imports informations */
	int Import(void);
	 
	albaString m_FileName;

	friend class albaOpImporterDICFileTest;
  
};
#endif
