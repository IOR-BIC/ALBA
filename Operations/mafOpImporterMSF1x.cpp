/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterMSF1x.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 11:55:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpImporterMSF1x.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafTagArray.h"
#include "mafMSFImporter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterMSF1x);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterMSF1x::mafOpImporterMSF1x(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  m_FileDir = "";//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterMSF1x::~mafOpImporterMSF1x( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterMSF1x::Copy()   
//----------------------------------------------------------------------------
{
  //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
  mafOpImporterMSF1x *cp = new mafOpImporterMSF1x(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterMSF1x::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = "MSF file (*.msf)|*.msf";
	mafString f;
  if(m_File.IsEmpty())
	{
		f = mafGetOpenFile(m_FileDir, wildc.GetCStr(), "Choose MSF file").c_str();
		m_File = f;
	}

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
	{
    wxSetWorkingDirectory(wxString(m_File.GetCStr()));
    ImportMSF();
	  result = OP_RUN_OK;
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpImporterMSF1x::ImportMSF()
//----------------------------------------------------------------------------
{
  bool success = false;
//	wxBusyInfo wait("Loading file: ...");
  
  m_Importer = new mafMSFImporter;
  m_Importer->SetURL(m_File);
  m_Importer->SetRoot((mafVMERoot *)m_Input->GetRoot());
  
  //mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,preader));
  mafTagItem item;
  mafVMERoot *root = (mafVMERoot *)m_Input->GetRoot();
  root->GetTagArray()->GetTag("APP_STAMP" , item);

  success = (m_Importer->Restore() == MAF_OK);

  if(!success)
    mafErrorMessage("I/O Error importing MSF file.");

  ((mafVMERoot *)m_Input->GetRoot())->GetStorage()->SetURL(m_File.GetCStr());
  ((mafVMERoot *)m_Input->GetRoot())->GetStorage()->ForceParserURL();
  ((mafVMERoot *)m_Input->GetRoot())->Update();

  root->GetTagArray()->SetTag("APP_STAMP", item.GetValue(), item.GetType());

  cppDEL(m_Importer);
}
