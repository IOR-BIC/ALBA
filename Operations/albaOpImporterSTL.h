/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterSTL
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterSTL_H__
#define __albaOpImporterSTL_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
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
EXPORT_STL_VECTOR(ALBA_EXPORT,int);
#endif

//----------------------------------------------------------------------------
// albaOpImporterSTL :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpImporterSTL: public albaOp
{
public:
	albaOpImporterSTL(const wxString &label = "STLImporter");
	~albaOpImporterSTL(); 
	
  albaTypeMacro(albaOpImporterSTL, albaOp);

  albaOp* Copy();

	/** Set the filename for the .stl to import */
  void SetFileName(const char *file_name);

  /** Builds operation's interface. */
	void OpRun();

  /** Makes the undo for the operation. */
  void OpUndo();

  /** Execute the operation. */
  void OpDo();

  /** Import the file. */
	void ImportSTL();

  /** Used to retrieve imported data. It is useful when the operation is executed from code.*/
  void GetImportedSTL(std::vector<albaVMESurface*> &importedSTL);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** Check if the STL file is binary */
  bool IsFileBinary(const char *name_file);

  /** Byte swapping of a 4-byte value */
  void Swap_Four(unsigned int *value);

  /** Check if the binary stl needs to be swapped. */
  void CheckSwap(const char *file_name, int &swapFlag);

  std::vector<albaVMESurface*> m_ImportedSTLs;
  std::vector<albaString>	    m_Files;
  std::vector<int>            m_Swaps;
};
#endif
