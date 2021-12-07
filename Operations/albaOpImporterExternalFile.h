/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterExternalFile
 Authors: Paolo Quadrani     Stefano Perticoni    Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterExternalFile_H__
#define __albaOpImporterExternalFile_H__

#include "albaOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaEvent;
class albaVMEExternalData;
class albaOp;

//----------------------------------------------------------------------------
// albaOpImporterExternalFile :
//----------------------------------------------------------------------------
/** Operation to import external files, such as Word or Excel files*/
class ALBA_EXPORT albaOpImporterExternalFile: public albaOp
{
public:
  albaOpImporterExternalFile(const wxString &label = "ExternalFileImporter");
 ~albaOpImporterExternalFile(); 

  albaTypeMacro(albaOpImporterExternalFile, albaOp);

  albaOp* Copy();

  /** Set the External filename to be imported. 
  This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void ImportExternalFile();

	/** Makes the undo for the operation. */
  void OpUndo();
	
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  albaString m_FileDir;
	albaString m_File;

	albaVMEExternalData *m_Vme; 
};
#endif
