/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMSF
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterMSF_H__
#define __albaOpImporterMSF_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// class forwarding:
//----------------------------------------------------------------------------
class albaVMEGroup;

//----------------------------------------------------------------------------
// albaOpImporterMSF :
//----------------------------------------------------------------------------
/** Import an existing MSF or ZMSF file while working to another one, to merge or compare data. */
class ALBA_EXPORT albaOpImporterMSF: public albaOp 
{
public:
  albaOpImporterMSF(const wxString &label = "MSFImporter");
  ~albaOpImporterMSF(); 

  albaTypeMacro(albaOpImporterMSF, albaOp);

  albaOp* Copy();
	
  /** Builds operation's interface. */
  void OpRun();

  /** Import MSF data. */
  int ImportMSF();

  /** Set the MSF filename to be imported. 
  This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

  /** Open the compressed zmsf filename. */
  const char *ZIPOpen(albaString filename);

  /** Remove temporary directory used for compressed msf files.*/
  void RemoveTempDirectory();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  albaString         m_File;
  albaString         m_TmpDir;

  albaVMEGroup      *m_Group;
  albaString         m_MSFFile;
};
#endif
