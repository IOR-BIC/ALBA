/*=========================================================================

Program: MAF2
Module:	 mafOpImporterAnsysCDBFile.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterAnsysCDBFile_H__
#define __mafOpImporterAnsysCDBFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafOpImporterAnsysCommon.h"

//----------------------------------------------------------------------------
// mafOpImporterAnsysCDBFile :
//----------------------------------------------------------------------------
/** 
Importer for Ansys CDB Input files
*/
class MAF_EXPORT mafOpImporterAnsysCDBFile : public mafOpImporterAnsysCommon
{
public:

	mafOpImporterAnsysCDBFile(const wxString &label = "MeshImporter");
	~mafOpImporterAnsysCDBFile(); 

	mafTypeMacro(mafOpImporterAnsysCDBFile, mafOp);

	mafOp* Copy();

protected:
		 
  mafString GetWildcard();

	int ParseAnsysFile(mafString fileName);
};
#endif
