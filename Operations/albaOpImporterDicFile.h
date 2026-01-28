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
#include "albaOpImporterFile.h"
#include "albaTextFileReaderHelper.h"
#include "albaDefines.h"


//----------------------------------------------------------------------------
// albaOpLoadScalarFromDicFile :
//----------------------------------------------------------------------------
/** 
Reads scalars from a .DIC file
*/
class ALBA_EXPORT albaOpImporterDicFile : public albaTextFileReaderHelper, public albaOpImporterFile
{
public:

	albaOpImporterDicFile(const wxString &label = "Load DIC file");
	~albaOpImporterDicFile(); 

	albaTypeMacro(albaOpImporterDicFile, albaOpImporterFile);

	/** Builds operation's interface. */
	void OpRun();

	albaOp* Copy();

	/** Get the filename to import */
	albaString GetFileName() const { return m_FileName; }

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** Imports informations, return ALBA_OK on success.  */
	int ImportFile();
	 
	friend class albaOpImporterDICFileTest;
  
};
#endif
