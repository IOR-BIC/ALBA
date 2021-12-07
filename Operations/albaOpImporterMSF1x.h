/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMSF1x
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterMSF1x_H__
#define __albaOpImporterMSF1x_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaMSFImporter;

//----------------------------------------------------------------------------
// albaOpImporterMSF1x :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpImporterMSF1x: public albaOp
{
public:
  albaOpImporterMSF1x(const wxString &label = "MSF1xImporter");
 ~albaOpImporterMSF1x(); 
  
  albaTypeMacro(albaOpImporterMSF1x, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

	/** Import MSF file. */
  void ImportMSF();

  /** Set the MSF filename to be imported. 
      This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = name;};

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

  albaString m_File;
  albaString m_FileDir;
  
  albaMSFImporter *m_Importer;
};
#endif
