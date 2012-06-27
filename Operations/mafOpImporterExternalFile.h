/*=========================================================================

 Program: MAF2
 Module: mafOpImporterExternalFile
 Authors: Paolo Quadrani     Stefano Perticoni    Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterExternalFile_H__
#define __mafOpImporterExternalFile_H__

#include "mafOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafEvent;
class mafVMEExternalData;
class mafOp;

//----------------------------------------------------------------------------
// mafOpImporterExternalFile :
//----------------------------------------------------------------------------
/** Operation to import external files, such as Word or Excel files*/
class MAF_EXPORT mafOpImporterExternalFile: public mafOp
{
public:
  mafOpImporterExternalFile(const wxString &label = "ExternalFileImporter");
 ~mafOpImporterExternalFile(); 

  mafTypeMacro(mafOpImporterExternalFile, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;};

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
  mafString m_FileDir;
	mafString m_File;

	mafVMEExternalData *m_Vme; 
};
#endif
