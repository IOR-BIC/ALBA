/*=========================================================================

Program: ALBA
Module: albaOpExporterAnsysCDBFile.h
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterAnsysCDBFile_H__
#define __albaOpExporterAnsysCDBFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaOpExporterAnsysCommon.h"

//----------------------------------------------------------------------------
// albaOpExporterAnsysInputFile :
//----------------------------------------------------------------------------

class ALBA_EXPORT albaOpExporterAnsysCDBFile : public albaOpExporterAnsysCommon
{
public:
	albaOpExporterAnsysCDBFile(const wxString &label = "albaOpExporterAnsysCDBFile");
	~albaOpExporterAnsysCDBFile(); 
	
  albaTypeMacro(albaOpExporterAnsysCDBFile, albaOp);
  
  /** Export the input mesh by writing it in Ansys .cdb format */
  int Write();
  
  albaOp* Copy();

protected:
  albaString GetWildcard();
  
  int WriteHeaderFile(FILE *file);
  int WriteNodesFile(FILE *file);
  int WriteElementsFile(FILE *file);
  int WriteMaterialsFile(FILE *file);  
};
#endif
