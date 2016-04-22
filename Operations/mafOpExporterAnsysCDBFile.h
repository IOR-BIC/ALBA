/*=========================================================================

Program: MAF2
Module: mafOpExporterAnsysCDBFile.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterAnsysCDBFile_H__
#define __mafOpExporterAnsysCDBFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafOpExporterAnsysCommon.h"

//----------------------------------------------------------------------------
// mafOpExporterAnsysInputFile :
//----------------------------------------------------------------------------

class MAF_EXPORT mafOpExporterAnsysCDBFile : public mafOpExporterAnsysCommon
{
public:
	mafOpExporterAnsysCDBFile(const wxString &label = "mafOpExporterAnsysCDBFile");
	~mafOpExporterAnsysCDBFile(); 
	
  mafTypeMacro(mafOpExporterAnsysCDBFile, mafOp);
  
  /** Export the input mesh by writing it in Ansys .cdb format */
  int Write();
  
  mafOp* Copy();

protected:
  mafString GetWildcard();
  
  int WriteHeaderFile(FILE *file);
  int WriteNodesFile(FILE *file);
  int WriteElementsFile(FILE *file);
  int WriteMaterialsFile(FILE *file);  
};
#endif
