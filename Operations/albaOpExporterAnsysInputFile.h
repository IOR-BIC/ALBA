/*=========================================================================

Program: ALBA
Module: albaOpExporterAnsysInputFile.h
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterAnsysInputFile_H__
#define __albaOpExporterAnsysInputFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------

#include "albaOpExporterAnsysCommon.h"

//----------------------------------------------------------------------------
// albaOpExporterAnsysInputFile :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpExporterAnsysInputFile : public albaOpExporterAnsysCommon
{
public:
	albaOpExporterAnsysInputFile(const wxString &label = "albaOpExporterAnsysInputFile");
	~albaOpExporterAnsysInputFile(); 
	
  albaTypeMacro(albaOpExporterAnsysInputFile, albaOp);

  /** Export the input mesh by writing it in Ansys .inp format */
  int Write();

  albaOp* Copy();

protected:
  albaString GetWildcard();

  int WriteHeaderFile(FILE *file );
  int WriteNodesFile(FILE *file );
  int WriteElementsFile(FILE *file);
  int WriteMaterialsFile(FILE *file);  
};
#endif
