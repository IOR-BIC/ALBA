/*=========================================================================
 Program: MAF2
 Module: mafOpExporterMSF
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpExporterMSF.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVME.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafVMEIterator.h"
#include "mafAbsLogicManager.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterMSF);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterMSF::mafOpExporterMSF(const wxString &label) :
mafOp(label)
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;

  m_MSFFile    = "";
  m_MSFFileDir = mafGetLastUserFolder().c_str();

}
//----------------------------------------------------------------------------
mafOpExporterMSF::~mafOpExporterMSF() { }

//----------------------------------------------------------------------------
bool mafOpExporterMSF::Accept(mafVME*vme)
{
	return (vme != NULL) && (!vme->IsA("mafVMERoot"));
}

//----------------------------------------------------------------------------
mafOp* mafOpExporterMSF::Copy()
{
	mafOpExporterMSF *cp = new mafOpExporterMSF(m_Label);
	cp->m_Listener = m_Listener;
	cp->m_MSFFile = m_MSFFile;
	cp->m_MSFFileDir = m_MSFFileDir;
	return cp;
}

//----------------------------------------------------------------------------
char ** mafOpExporterMSF::GetIcon()
{
#include "pic/MENU_IMPORT_MSF.xpm"
	return MENU_IMPORT_MSF_xpm;
}

//----------------------------------------------------------------------------
void mafOpExporterMSF::OpRun()   
{
	mafString wildc;
	const char *ext = GetLogicManager()->GetMsfFileExtension();
	
	wildc.Printf("Project File (*.%s)|*.%s", ext, ext);
	
	mafString f;
  if (m_MSFFile.IsEmpty())
  {
    f = mafGetSaveFile(m_MSFFileDir.GetCStr(),wildc.GetCStr()).c_str(); 
    m_MSFFile = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_MSFFile.IsEmpty()) 
	{
    if (ExportMSF() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
	}
	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
int mafOpExporterMSF::ExportMSF()
{					
  if (!m_TestMode)
  {
  	wxBusyInfo wait("Saving MSF: Please wait");
  }
  assert(m_MSFFile != "");

	if(!wxFileExists(wxString(m_MSFFile)))
	{
		wxString path, name, ext, dir2;
		wxSplitPath(m_MSFFile.GetCStr(),&path,&name,&ext);
		dir2 = path + "\\" + name;
		if(!wxDirExists(dir2))
			wxMkdir(dir2);
		m_MSFFile = dir2 + "\\" + name + "." + ext;
	}

	return StorageFile();
}

//----------------------------------------------------------------------------
int mafOpExporterMSF::StorageFile()
{
	mafVMEStorage storage;
	storage.SetURL(m_MSFFile.GetCStr());
	storage.GetRoot()->SetName("root");
	storage.SetListener(this);
	storage.GetRoot()->Initialize();
	
	// Copy Tree 
	mafVME::CopyTree(m_Input, storage.GetRoot());
	
	// Set AbsMatrix
	storage.GetRoot()->GetFirstChild()->SetAbsMatrix(*m_Input->GetOutput()->GetAbsMatrix());  //Paolo 5-5-2004

	// Store
	if (storage.Store() != 0)
	{
		if (!m_TestMode)
		{
			wxMessageBox("Error while exporting MSF");
		}
		return MAF_ERROR;
	}

	return MAF_OK;
}