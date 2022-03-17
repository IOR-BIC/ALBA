/*=========================================================================
Program:   AssemblerPro
Module:    albaOpImportProsthesisToDB.cpp
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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpImportProsthesisToDB.h"
#include "albaGUI.h"

#include "albaGUI.h"
#include "albaProsthesesDBManager.h"
#include "albaVME.h"

#include "wx/wfstream.h"
#include "wx/zipstrm.h"
#include <wx/busyinfo.h>
#include <wx/fs_zip.h>
#include <wx/zstream.h>
#include <fstream>
#include <iosfwd>
#include <xmemory>
#include "albaLogicWithManagers.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImportProsthesisToDB);

//----------------------------------------------------------------------------
albaOpImportProsthesisToDB::albaOpImportProsthesisToDB(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;

	m_ProsthesesDBManager = NULL;
	m_AuxProsthesesDBManager = NULL;
}
//----------------------------------------------------------------------------
albaOpImportProsthesisToDB::~albaOpImportProsthesisToDB()
{
}

//----------------------------------------------------------------------------
bool albaOpImportProsthesisToDB::InternalAccept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
char** albaOpImportProsthesisToDB::GetIcon()
{
#include "pic/MENU_OP_IMPORT_PROSTHESIS.xpm"
	return MENU_OP_IMPORT_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* albaOpImportProsthesisToDB::Copy()
{
	albaOpImportProsthesisToDB *cp = new albaOpImportProsthesisToDB(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpImportProsthesisToDB::OpRun()
{
	m_ProsthesesDBManager = GetLogicManager()->GetProsthesesDBManager();
	m_AuxProsthesesDBManager = new albaProsthesesDBManager();

	int nProsthesis = m_ProsthesesDBManager->GetProstheses().size();
	int nProducer = m_ProsthesesDBManager->GetProducers().size();
	int nType = m_ProsthesesDBManager->GetTypes().size();

	// Select File
	albaString wildc = "ZIP file (*.zip)|*.zip|XML file (*.xml)|*.xml";
	wxString prosthesisFile = albaGetOpenFile(albaGetLastUserFolder().c_str(), wildc, "Select file").c_str();

	int result = ImportDB(prosthesisFile);

	if (result == OP_RUN_OK)
	{
		nProsthesis = m_ProsthesesDBManager->GetProstheses().size() - nProsthesis;
		nProducer = m_ProsthesesDBManager->GetProducers().size() - nProducer;
		nType = m_ProsthesesDBManager->GetTypes().size() - nType;

		wxString message = wxString::Format("Added %d Prosthesis, %d Producers, %d Types", nProsthesis, nProducer, nType);
		albaMessage(message);
	}

	delete m_AuxProsthesesDBManager;

	OpStop(result);
}
//----------------------------------------------------------------------------
void albaOpImportProsthesisToDB::OpStop(int result)
{	
	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpImportProsthesisToDB::OpDo()
{
}

//----------------------------------------------------------------------------
int albaOpImportProsthesisToDB::ImportDB(wxString dbFile)
{
	int result = OP_RUN_CANCEL;

	if (wxFileExists(dbFile))
	{
		wxString path, name, ext;
		wxSplitPath(dbFile.c_str(), &path, &name, &ext);

		if (ext == "zip")
		{
			result = ImportDBFromZip(dbFile);
		}
		else if (ext == "xml")
		{
			result = ImportDBFromXml(dbFile);
		}
	}

	return result;
}

//----------------------------------------------------------------------------
int albaOpImportProsthesisToDB::ImportDBFromZip(wxString &dbZipFile)
{
	wxString dest = m_ProsthesesDBManager->GetDBDir().GetCStr();
	
	// Open the zip file to read the prosthesis information
	std::vector<albaString> filesVect = ExtractZipFiles(dbZipFile, dest);
	if (filesVect.size() == 0)
	{
		wxMessageBox("Error Open DB!");
		return OP_RUN_CANCEL;
	}

	// Import xml Files
	int result = OP_RUN_OK;
	
	for (int i = 0; i < filesVect.size(); i++)
	{
		wxString path, name, ext;
		wxSplitPath(filesVect[i].GetCStr(), &path, &name, &ext);

		if (ext == "xml")
		{
			wxString xmlFile = filesVect[i].GetCStr();
			result = ImportDBFromXml(xmlFile);

			// Destroy xml file
			wxRemoveFile(xmlFile);
		}
	}

	return result;
}
//----------------------------------------------------------------------------
int albaOpImportProsthesisToDB::ImportDBFromXml(wxString &dbXmlFile)
{	
	m_AuxProsthesesDBManager->SetDBDir(m_ProsthesesDBManager->GetDBDir());
	m_AuxProsthesesDBManager->LoadDBFromFile(dbXmlFile);

	// Producers
	for (int p =0; p < m_AuxProsthesesDBManager->GetProducers().size(); p++)
	{
		albaProDBProducer *producer = m_AuxProsthesesDBManager->GetProducers()[p];

		if (!IsInDB(producer))
			m_ProsthesesDBManager->AddProducer(producer);
	}

	// Types
	for (int t = 0; t < m_AuxProsthesesDBManager->GetTypes().size(); t++)
	{
		albaProDBType *type = m_AuxProsthesesDBManager->GetTypes()[t];

		if (!IsInDB(type))
			m_ProsthesesDBManager->AddType(type);
	}

	// Prosthesis
	for (int p = 0; p < m_AuxProsthesesDBManager->GetProstheses().size(); p++)
	{
		albaProDBProsthesis *prosthesis = m_AuxProsthesesDBManager->GetProstheses()[p];

		if (!IsInDB(prosthesis))
			m_ProsthesesDBManager->AddProsthesis(prosthesis);
	}

	m_ProsthesesDBManager->SaveDB();

	return OP_RUN_OK;
}
//----------------------------------------------------------------------------
std::vector<albaString> albaOpImportProsthesisToDB::ExtractZipFiles(const wxString &ZipFile, const wxString &TargetDir)
{
	bool ret = true;
	std::vector<albaString> listFiles;

	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxFileSystem fs;
	std::auto_ptr<wxZipEntry> entry(new wxZipEntry);

	do {
		wxFileInputStream in(ZipFile);
		if (!in)
		{
			wxLogError(_T("Can not open file '") + ZipFile + _T("'."));
			ret = false;
			break;
		}
		wxZipInputStream zip(in);

		while (entry.reset(zip.GetNextEntry()), entry.get() != NULL)
		{
			// Access meta-data
			wxString name = entry->GetName();
			name = TargetDir + name;

			// Read 'zip' to access the entry's data
			if (entry->IsDir())
			{
				int perm = entry->GetMode();
				wxFileName::Mkdir(name, perm, wxPATH_MKDIR_FULL);
			}
			else // it is a file
			{
				zip.OpenEntry(*entry.get());
				if (!zip.CanRead())
				{
					wxLogError(_T("Can not read zip entry '") + entry->GetName() + _T("'."));
					ret = false;
					break;
				}
				wxFileOutputStream file(name);
				if (!file)
				{
					wxLogError(_T("Can not create file '") + name + _T("'."));
					ret = false;
					break;
				}
				else
				{
					listFiles.push_back(name);
				}

				zip.Read(file);
			}
		}
	} while (false);

	return listFiles;
}

// Utilities
//----------------------------------------------------------------------------
bool albaOpImportProsthesisToDB::IsInDB(albaProDBProducer *producer)
{
	bool result = false;

	for (int p = 0; p < m_ProsthesesDBManager->GetProducers().size(); p++)
	{
		albaProDBProducer *pro = m_ProsthesesDBManager->GetProducers()[p];

		if (producer->GetName() == pro->GetName() /*|| producer->GetWebSite() == pro->GetWebSite() || producer->GetImgFileName() == pro->GetImgFileName()*/)
			return true;
	}

	return result;
}
//----------------------------------------------------------------------------
bool albaOpImportProsthesisToDB::IsInDB(albaProDBType *type)
{
	bool result = false;

	for (int t = 0; t < m_ProsthesesDBManager->GetTypes().size(); t++)
	{
		albaProDBType *typ = m_ProsthesesDBManager->GetTypes()[t];

		if (type->GetName() == typ->GetName())
			return true;
	}

	return result;
}
//----------------------------------------------------------------------------
bool albaOpImportProsthesisToDB::IsInDB(albaProDBProsthesis *prosthesis)
{
	bool result = false;

	for (int p = 0; p < m_ProsthesesDBManager->GetProstheses().size(); p++)
	{
		albaProDBProsthesis *pro = m_ProsthesesDBManager->GetProstheses()[p];

		if (prosthesis->GetName() == pro->GetName() 
				|| prosthesis->GetSide() == pro->GetSide()
			//|| prosthesis->GetProducer() == pro->GetProducer()
			//|| prosthesis->GetType() == pro->GetType()
			//|| prosthesis->GetBendingAngle() == pro->GetBendingAngle()
			)
			return true;
	}

	return result;
}