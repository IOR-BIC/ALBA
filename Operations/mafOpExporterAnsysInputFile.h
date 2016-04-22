/*=========================================================================

Program: MAF2
Module: mafOpExporterAnsysInputFile.h
Authors: Nicola Vanella

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterAnsysInputFile_H__
#define __mafOpExporterAnsysInputFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------

#include "mafOpExporterAnsysCommon.h"

//----------------------------------------------------------------------------
// mafOpExporterAnsysInputFile :
//----------------------------------------------------------------------------
class MAF_EXPORT mafOpExporterAnsysInputFile : public mafOpExporterAnsysCommon
{
public:
	mafOpExporterAnsysInputFile(const wxString &label = "mafOpExporterAnsysInputFile");
	~mafOpExporterAnsysInputFile(); 
	
  mafTypeMacro(mafOpExporterAnsysInputFile, mafOp);

  /** Export the input mesh by writing it in Ansys .inp format */
  int Write();

  mafOp* Copy();

protected:
  mafString GetWildcard();

  int WriteHeaderFile(FILE *file );
  int WriteNodesFile(FILE *file );
  int WriteElementsFile(FILE *file);
  int WriteMaterialsFile(FILE *file);  
};
#endif
