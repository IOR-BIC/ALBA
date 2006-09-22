/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMSF1xImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-22 10:11:57 $
  Version:   $Revision: 1.5 $
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

#include "mmoMSF1xImporter.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafTagArray.h"
#include "mafMSFImporter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoMSF1xImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoMSF1xImporter::mmoMSF1xImporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoMSF1xImporter::~mmoMSF1xImporter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mmoMSF1xImporter::Copy()   
//----------------------------------------------------------------------------
{
  //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
  mmoMSF1xImporter *cp = new mmoMSF1xImporter(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mmoMSF1xImporter::OpRun()   
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
void mmoMSF1xImporter::ImportMSF()
//----------------------------------------------------------------------------
{
  bool success = false;
//	wxBusyInfo wait("Loading file: ...");
  
  m_Importer = new mafMSFImporter;
  m_Importer->SetURL(m_File);
  m_Importer->SetRoot((mafVMERoot *)m_Input->GetRoot());
  
  //mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,preader));
  
  success = (m_Importer->Restore() == MAF_OK);

  if(!success)
    mafErrorMessage("I/O Error importing MSF file.");

  ((mafVMERoot *)m_Input->GetRoot())->GetStorage()->SetURL(m_File.GetCStr());
  ((mafVMERoot *)m_Input->GetRoot())->GetStorage()->ForceParserURL();
  ((mafVMERoot *)m_Input->GetRoot())->Update();

  cppDEL(m_Importer);
}
