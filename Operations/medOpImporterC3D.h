/*=========================================================================

 Program: MAF2
 Module: medOpImporterC3D
 Authors: Paolo Quadrani - porting Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpImporterC3D_H__
#define __medOpImporterC3D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafNode;
class mafEvent;
class mafEventListener;
//----------------------------------------------------------------------------
// medOpImporterC3D :
//----------------------------------------------------------------------------
/**
class name: medOpImporterC3D
Import C3D file inside a landmark cloud. C3D is a standard format file
for movement analysis data. http://www.c3d.org/
*/
class MAF_EXPORT medOpImporterC3D: public mafOp
{
public:
  /** constructor */
  medOpImporterC3D(wxString label);
  /** destructor */
 ~medOpImporterC3D();
  /** retrieve the copy of the object */
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme) {return true;};

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

protected:
  mafVME  *m_Vme; 
	wxString m_File;
	wxString m_FileDir;
	wxString m_Dict; 
	wxString m_DictDir;
	int m_DictionaryAvailable;
};
#endif
