/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporter
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporter_H__
#define __albaOpImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;

//----------------------------------------------------------------------------
// albaOpImporter :
//----------------------------------------------------------------------------
/** 
Base importer operation that extends albaOp with virtual import methods.
By extending this class and implementing the ImportFile() method to import the file 
using m_FileName, drag and drop functionality is automatically enabled in ALBA applications.
*/
class ALBA_EXPORT albaOpImporterFile : public albaOp
{
public:
	albaOpImporterFile(const wxString &label = "Importer");
	~albaOpImporterFile();

	
	/** Set the file to be imported. */
	virtual void SetFileName(albaString fileName);

	/** Accept file for import validation. */
  int AcceptFile(albaString fileName);
	
	/** Import the file, return ALBA_OK on success. */
	virtual int ImportFile()=0;

	/** Retrieve the file name*/
	albaString GetFileName() { return m_FileName; };

protected:

	/** Set the wildcard for the Accept file method. */
	void SetWildc(albaString wildc);

	albaString m_Wildc;
	albaString m_FileName;
};

#endif