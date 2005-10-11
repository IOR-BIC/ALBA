/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVRMLImporter.h,v $
  Language:  C++
  Date:      $Date: 2005-10-11 13:01:05 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVRMLImporter_H__
#define __mmoVRMLImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEGroup;

//----------------------------------------------------------------------------
//mmoVRMLImporter :
//----------------------------------------------------------------------------
/** */
class mmoVRMLImporter: public mafOp
{
public:
           mmoVRMLImporter(wxString label);
	virtual ~mmoVRMLImporter();
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .vrl to import */
  void SetFileName(const char *file_name);

  /** Import vrml data. */
  void ImportVRML();

protected:
	wxString m_File;
	wxString m_FileDir;
  
	mafVMEGroup *m_Group;
};
#endif
