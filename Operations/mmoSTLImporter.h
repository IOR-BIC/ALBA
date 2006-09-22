/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoSTLImporter.h,v $
  Language:  C++
  Date:      $Date: 2006-09-22 10:11:57 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani     
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoSTLImporter_H__
#define __mmoSTLImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMESurface;
class mafEvent;

//----------------------------------------------------------------------------
// mmoSTLImporter :
//----------------------------------------------------------------------------
/** */
class mmoSTLImporter: public mafOp
{
public:
	mmoSTLImporter(wxString label = "STLImporter");
	~mmoSTLImporter(); 
	
  mafTypeMacro(mmoSTLImporter, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Set the filename for the .stl to import */
  void SetFileName(const char *file_name);

  /** Builds operation's interface. */
	void OpRun();

	/** Import the file. */
	void ImportSTL();

protected:
  /** Check if the STL file is binary */
  bool IsFileBinary(const char *name_file);

  /** Byte swapping of a 4-byte value */
  void Swap_Four(unsigned int *value);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Check if the binary stl needs to be swapped. */
  void CheckSwap(const char *file_name);

	mafVMESurface *m_ImportedSTL;
  mafString	m_File;
	mafString	m_FileDir;
	int			  m_Swap;	
};
#endif
