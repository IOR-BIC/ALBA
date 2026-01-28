/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVRML
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterVRML_H__
#define __albaOpImporterVRML_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOpImporterFile.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEGroup;

//----------------------------------------------------------------------------
//albaOpImporterVRML :
//----------------------------------------------------------------------------
/**
	VRML files are commonly called worlds and have the .wrl extension. 
	Although VRML worlds use a text format they may often be compressed using gzip so 
	that they transfer over the internet more quickly. 
	This modality in not supported by alba vrml importer, it can only import uncompressed wrl.

*/
class ALBA_EXPORT albaOpImporterVRML: public albaOpImporterFile
{
public:
           albaOpImporterVRML(const wxString &label = "VRMLImporter");
	virtual ~albaOpImporterVRML();
	
  albaTypeMacro(albaOpImporterVRML, albaOpImporterFile);

  albaOp* Copy();

	/** Return true for the acceptable vme type. */


	/** Builds operation's interface. */
  void OpRun();

  /** Set the filename for the .vrl to import */
  void SetFileName(const char *file_name);

  /** Import vrml data. */
  void ImportVRML();
  /** Import the file, return ALBA_OK on success. */
  virtual int ImportFile();



protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME* node) { return true; };

	albaString m_FileDir;
  
	albaVMEGroup *m_Group;
};
#endif
