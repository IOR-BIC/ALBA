/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterPLY
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterPLY_H__
#define __albaOpImporterPLY_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOpImporterFile.h"
#include "albaString.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMESurface;
class albaEvent;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaString);
EXPORT_STL_VECTOR(ALBA_EXPORT,albaVMESurface*);
#endif

//----------------------------------------------------------------------------
// albaOpImporterPLY :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpImporterPLY: public albaOpImporterFile
{
public:
	albaOpImporterPLY(const wxString &label = "PLYImporter");
	~albaOpImporterPLY(); 
	
  albaTypeMacro(albaOpImporterPLY, albaOpImporterFile);

  albaOp* Copy();

	/** Set the filename for the .stl to import */
  void SetFileName(albaString filename);

  /** Builds operation's interface. */
	void OpRun();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Execute the operation. */
  void OpDo();

  /** Import the file, return ALBA_OK on success. */
	int ImportFile();

  /** Used to retrieve imported data. It is useful when the operation is executed from code.*/
  void GetImportedPLY(std::vector<albaVMESurface*> &importedPLY);
	
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);
	
	std::vector<albaVMESurface*> m_ImportedPLYs;
  std::vector<albaString>	    m_Files;
};
#endif
