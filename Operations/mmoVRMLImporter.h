/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVRMLImporter.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.6 $
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
/**
	VRML files are commonly called worlds and have the .wrl extension. 
	Although VRML worlds use a text format they may often be compressed using gzip so 
	that they transfer over the internet more quickly. 
	This modality in not supported by maf vrml importer, it can only import uncompressed wrl.

*/
class mmoVRMLImporter: public mafOp
{
public:
           mmoVRMLImporter(const wxString &label = "VRMLImporter");
	virtual ~mmoVRMLImporter();
	
  mafTypeMacro(mmoVRMLImporter, mafOp);

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
	mafString m_File;
	mafString m_FileDir;
  
	mafVMEGroup *m_Group;
};
#endif
