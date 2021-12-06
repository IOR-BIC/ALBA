/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMSF
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExporterMSF_H__
#define __albaOpExporterMSF_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaEvent;

//----------------------------------------------------------------------------
// albaOpExporterMSF :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpExporterMSF: public albaOp
{
public:
  albaOpExporterMSF(const wxString &label = "MSFExporter");
 ~albaOpExporterMSF(); 
  
  albaTypeMacro(albaOpExporterMSF, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .msf to export */
  void SetFileName(const char *file_name) {m_MSFFile = file_name;};

  /** Get the filename for the .msf to export */
  albaString GetFileName() {return m_MSFFile;};

  /** Export selected subtree-tree to a .msf file*/
  int ExportMSF();
	
	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	// Prepare and Storage file
	int StorageFile();

  albaString m_MSFFile;
	albaString m_MSFFileDir;
};
#endif
