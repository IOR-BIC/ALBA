/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterExternalFile.h,v $
  Language:  C++
  Date:      $Date: 2007-09-17 11:25:54 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani     Stefano Perticoni    Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
class mafOpImporterExternalFile: public mafOp
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
