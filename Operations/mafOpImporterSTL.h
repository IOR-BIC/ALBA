/*=========================================================================

 Program: MAF2
 Module: mafOpImporterSTL
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterSTL_H__
#define __mafOpImporterSTL_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafString.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMESurface;
class mafEvent;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafString);
EXPORT_STL_VECTOR(MAF_EXPORT,mafVMESurface*);
EXPORT_STL_VECTOR(MAF_EXPORT,int);
#endif

//----------------------------------------------------------------------------
// mafOpImporterSTL :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpImporterSTL: public mafOp
{
public:
	mafOpImporterSTL(const wxString &label = "STLImporter");
	~mafOpImporterSTL(); 
	
  mafTypeMacro(mafOpImporterSTL, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME *node);

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
  void GetImportedSTL(std::vector<mafVMESurface*> &importedSTL);

protected:
  /** Check if the STL file is binary */
  bool IsFileBinary(const char *name_file);

  /** Byte swapping of a 4-byte value */
  void Swap_Four(unsigned int *value);

  /** Check if the binary stl needs to be swapped. */
  void CheckSwap(const char *file_name, int &swapFlag);

  std::vector<mafVMESurface*> m_ImportedSTLs;
  std::vector<mafString>	    m_Files;
  std::vector<int>            m_Swaps;
};
#endif
