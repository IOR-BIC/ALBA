/*=========================================================================

Program: ALBA
Module: albaOpImporterAnsysInputFile.h
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterAnsysInputFile_H__
#define __albaOpImporterAnsysInputFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOpImporterAnsysCommon.h"

//----------------------------------------------------------------------------
// albaOpImporterAnsysInputFile :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpImporterAnsysInputFile : public albaOpImporterAnsysCommon
{
public:
	albaOpImporterAnsysInputFile(const wxString &label = "MeshImporter");
	~albaOpImporterAnsysInputFile(); 
	
  albaTypeMacro(albaOpImporterAnsysInputFile, albaOpImporterFile);

  albaOp* Copy();

protected:

  int ParseAnsysFile(albaString fileName);

  int UpdateNodesFile(FILE *outFile);
  int UpdateElements();
};
#endif
