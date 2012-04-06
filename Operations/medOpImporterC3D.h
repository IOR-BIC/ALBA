/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterC3D.h,v $
  Language:  C++
  Date:      $Date: 2012-04-06 09:13:46 $
  Version:   $Revision: 1.1.2.3 $
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
#include "medOperationsDefines.h"
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
class MED_OPERATION_EXPORT medOpImporterC3D: public mafOp
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
