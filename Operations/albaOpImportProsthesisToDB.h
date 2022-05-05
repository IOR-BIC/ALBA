/*=========================================================================
Program:   AssemblerPro
Module:    albaOpImportProsthesisToDB.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpImportProsthesisToDB_H__
#define __albaOpImportProsthesisToDB_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaProsthesesDBManager.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: albaOpImportProsthesisToDB
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpImportProsthesisToDB : public albaOp
{
public:
	/** Constructor. */
	albaOpImportProsthesisToDB(wxString label = "Import Prosthesis DB");

	/** Destructor. */
	~albaOpImportProsthesisToDB();

	/** RTTI macro. */
	albaTypeMacro(albaOpImportProsthesisToDB, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	int ImportDB(wxString dbFile);

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	int ImportDBFromZip(wxString &dbZipFile);
	int ImportDBFromXml(wxString &dbXmlFile);
	
	std::vector<albaString> ExtractZipFiles(const wxString &ZipFile, const wxString &TargetDir);

	bool IsInDB(albaProDBProducer *producer);
	bool IsInDB(albaProDBType *type);
	bool IsInDB(albaProDBProsthesis *prosthesis);

	albaProsthesesDBManager *m_ProsthesesDBManager;
	albaProsthesesDBManager *m_AuxProsthesesDBManager;
};
#endif
