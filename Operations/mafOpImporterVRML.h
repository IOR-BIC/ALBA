/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVRML
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterVRML_H__
#define __mafOpImporterVRML_H__

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
//mafOpImporterVRML :
//----------------------------------------------------------------------------
/**
	VRML files are commonly called worlds and have the .wrl extension. 
	Although VRML worlds use a text format they may often be compressed using gzip so 
	that they transfer over the internet more quickly. 
	This modality in not supported by maf vrml importer, it can only import uncompressed wrl.

*/
class MAF_EXPORT mafOpImporterVRML: public mafOp
{
public:
           mafOpImporterVRML(const wxString &label = "VRMLImporter");
	virtual ~mafOpImporterVRML();
	
  mafTypeMacro(mafOpImporterVRML, mafOp);

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
