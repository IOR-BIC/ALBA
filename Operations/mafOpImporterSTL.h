/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterSTL.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani     
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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

//----------------------------------------------------------------------------
// mafOpImporterSTL :
//----------------------------------------------------------------------------
/** */
class mafOpImporterSTL: public mafOp
{
public:
	mafOpImporterSTL(const wxString &label = "STLImporter");
	~mafOpImporterSTL(); 
	
  mafTypeMacro(mafOpImporterSTL, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

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
	mafString	                  m_FileDir;
  std::vector<int>            m_Swaps;
};
#endif
