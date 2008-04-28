/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterC3D.h,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:38:31 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani - porting Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpImporterC3D_H__
#define __medOpImporterC3D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
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
*/
class medOpImporterC3D: public mafOp
{
public:
  medOpImporterC3D(wxString label);
 ~medOpImporterC3D(); 
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
