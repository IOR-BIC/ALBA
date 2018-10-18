/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMSF
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterMSF_H__
#define __mafOpImporterMSF_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// class forwarding:
//----------------------------------------------------------------------------
class mafVMEGroup;

//----------------------------------------------------------------------------
// mafOpImporterMSF :
//----------------------------------------------------------------------------
/** Import an existing MSF or ZMSF file while working to another one, to merge or compare data. */
class MAF_EXPORT mafOpImporterMSF: public mafOp 
{
public:
  mafOpImporterMSF(const wxString &label = "MSFImporter");
  ~mafOpImporterMSF(); 

  mafTypeMacro(mafOpImporterMSF, mafOp);

  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafVME* node) {return true;};

  /** Builds operation's interface. */
  void OpRun();

  /** Import MSF data. */
  int ImportMSF();

  /** Set the MSF filename to be imported. 
  This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

  /** Open the compressed zmsf filename. */
  const char *ZIPOpen(mafString filename);

  /** Remove temporary directory used for compressed msf files.*/
  void RemoveTempDirectory();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:
  mafString         m_File;
  mafString         m_TmpDir;

  mafVMEGroup      *m_Group;
  mafString         m_MSFFile;
};
#endif
