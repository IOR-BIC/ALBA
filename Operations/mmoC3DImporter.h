/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoC3DImporter.h,v $
  Language:  C++
  Date:      $Date: 2007-03-09 11:32:14 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani - porting Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoC3DImporter_H__
#define __mmoC3DImporter_H__

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
// mmoC3DImporter :
//----------------------------------------------------------------------------
/**
*/
class mmoC3DImporter: public mafOp
{
public:
  mmoC3DImporter(wxString label);
 ~mmoC3DImporter(); 
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
