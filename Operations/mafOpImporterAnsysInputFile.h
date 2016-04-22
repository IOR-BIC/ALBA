/*=========================================================================

Program: MAF2
Module: mafOpImporterAnsysInputFile.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterAnsysInputFile_H__
#define __mafOpImporterAnsysInputFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOpImporterAnsysCommon.h"

//----------------------------------------------------------------------------
// mafOpImporterAnsysInputFile :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpImporterAnsysInputFile : public mafOpImporterAnsysCommon
{
public:
	mafOpImporterAnsysInputFile(const wxString &label = "MeshImporter");
	~mafOpImporterAnsysInputFile(); 
	
  mafTypeMacro(mafOpImporterAnsysInputFile, mafOp);

  mafOp* Copy();

protected:

  mafString GetWildcard();

  int ParseAnsysFile(mafString fileName);

  int UpdateNodesFile(FILE *outFile);
  int UpdateElements();
};
#endif
