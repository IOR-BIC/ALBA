/*=========================================================================

Program: ALBA
Module:	 albaOpImporterAnsysCDBFile.h
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterAnsysCDBFile_H__
#define __albaOpImporterAnsysCDBFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOpImporterFile.h"
#include "albaOpImporterAnsysCommon.h"

//----------------------------------------------------------------------------
// albaOpImporterAnsysCDBFile :
//----------------------------------------------------------------------------
/** 
Importer for Ansys CDB Input files
*/
class ALBA_EXPORT albaOpImporterAnsysCDBFile : public albaOpImporterAnsysCommon
{
public:

	albaOpImporterAnsysCDBFile(const wxString &label = "MeshImporter");
	~albaOpImporterAnsysCDBFile(); 

	albaTypeMacro(albaOpImporterAnsysCDBFile, albaOpImporterFile);

	albaOp* Copy();

protected:
	int ParseAnsysFile(albaString fileName);
};
#endif
