/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMSFExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-22 10:11:57 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoMSFExporter.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"

#include "mafNode.h"
#include "mafVME.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoMSFExporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoMSFExporter::mmoMSFExporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;

  m_MSFFile    = "";
	m_MSFFileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoMSFExporter::~mmoMSFExporter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoMSFExporter::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = "MAF Storage Format (*.msf)|*.msf";
	mafString f;
  if (m_MSFFile.IsEmpty())
  {
    f = mafGetSaveFile(m_MSFFileDir.GetCStr(),wildc.GetCStr()).c_str(); 
    m_MSFFile = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_MSFFile.IsEmpty()) 
	{
    ExportMSF();
		result = OP_RUN_OK;
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoMSFExporter::ExportMSF()
//----------------------------------------------------------------------------
{					
  wxBusyInfo wait("Saving MSF: Please wait");
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

  mafVMEStorage storage;
  storage.SetURL(m_MSFFile.GetCStr());
  storage.GetRoot()->SetName("root");
  storage.SetListener(this);
  storage.GetRoot()->Initialize();
//  mafVME *parent = (mafVME *)m_Input->GetParent();
//  m_Input->ReparentTo(storage.GetRoot());
	mafNode::CopyTree(m_Input, storage.GetRoot());
  ((mafVME *)storage.GetRoot()->GetFirstChild())->SetAbsMatrix(*((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());  //Paolo 5-5-2004
  if (storage.Store()!=0)
    wxMessageBox("Error while exporting MSF");
//  m_Input->ReparentTo(parent);
}
//----------------------------------------------------------------------------
mafOp* mmoMSFExporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoMSFExporter *cp= new mmoMSFExporter(m_Label);
  cp->m_Listener    = m_Listener;
  cp->m_MSFFile     = m_MSFFile;
  cp->m_MSFFileDir  = m_MSFFileDir;
  return cp;
}
