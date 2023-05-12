/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMSF
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpExporterMSF.h"
#include "wx/busyinfo.h"

#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVME.h"
#include "albaVME.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEIterator.h"
#include "albaAbsLogicManager.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterMSF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterMSF::albaOpExporterMSF(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;

  m_MSFFile    = "";
  m_MSFFileDir = albaGetLastUserFolder().c_str();

}
//----------------------------------------------------------------------------
albaOpExporterMSF::~albaOpExporterMSF() { }

//----------------------------------------------------------------------------
bool albaOpExporterMSF::InternalAccept(albaVME*vme)
{
	return (vme != NULL) && (!vme->IsA("albaVMERoot"));
}

//----------------------------------------------------------------------------
albaOp* albaOpExporterMSF::Copy()
{
	albaOpExporterMSF *cp = new albaOpExporterMSF(m_Label);
	cp->m_Listener = m_Listener;
	cp->m_MSFFile = m_MSFFile;
	cp->m_MSFFileDir = m_MSFFileDir;
	return cp;
}

//----------------------------------------------------------------------------
char ** albaOpExporterMSF::GetIcon()
{
#include "pic/MENU_IMPORT_ALBA.xpm"
	return MENU_IMPORT_ALBA_xpm;
}

//----------------------------------------------------------------------------
void albaOpExporterMSF::OpRun()   
{
	albaString wildc;
	const char *ext = GetLogicManager()->GetMsfFileExtension();
	
	wildc.Printf("Project File (*.%s)|*.%s", ext, ext);
	
	albaString f;
  if (m_MSFFile.IsEmpty())
  {
    f = albaGetSaveFile(m_MSFFileDir.GetCStr(),wildc.GetCStr()).c_str(); 
    m_MSFFile = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_MSFFile.IsEmpty()) 
	{
    if (ExportMSF() == ALBA_OK)
    {
      result = OP_RUN_OK;
    }
	}
	albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
int albaOpExporterMSF::ExportMSF()
{					
  if (!m_TestMode)
  {
  	wxBusyInfo wait("Saving Project: Please wait");
  }
  assert(m_MSFFile != "");

	if(!wxFileExists(wxString(m_MSFFile)))
	{
		wxString path, name, ext, dir2;
		wxFileName::SplitPath(m_MSFFile.GetCStr(),&path,&name,&ext);
		dir2 = path + "\\" + name;
		if(!wxDirExists(dir2))
			wxMkdir(dir2);
		m_MSFFile = dir2 + "\\" + name + "." + ext;
	}

	return StorageFile();
}

//----------------------------------------------------------------------------
int albaOpExporterMSF::StorageFile()
{
	albaVMEStorage storage;
	storage.SetURL(m_MSFFile.GetCStr());
	storage.GetRoot()->SetName("root");
	storage.SetListener(this);
	storage.GetRoot()->Initialize();
	
	// Copy Tree 
	albaVME::CopyTree(m_Input, storage.GetRoot());
	
	// Set AbsMatrix
	storage.GetRoot()->GetFirstChild()->SetAbsMatrix(*m_Input->GetOutput()->GetAbsMatrix());  //Paolo 5-5-2004

	// Store
	if (storage.Store() != 0)
	{
		if (!m_TestMode)
		{
			wxMessageBox("Error while exporting MSF");
		}
		return ALBA_ERROR;
	}

	return ALBA_OK;
}