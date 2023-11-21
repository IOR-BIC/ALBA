/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMSF1x
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

#include "albaOpImporterMSF1x.h"
#include <wx/busyinfo.h>
#include "albaEvent.h"

#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaTagArray.h"
#include "albaMSFImporter.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterMSF1x);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterMSF1x::albaOpImporterMSF1x(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  m_FileDir = albaGetLastUserFolder();
}
//----------------------------------------------------------------------------
albaOpImporterMSF1x::~albaOpImporterMSF1x( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterMSF1x::Copy()   
//----------------------------------------------------------------------------
{
  //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
  albaOpImporterMSF1x *cp = new albaOpImporterMSF1x(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterMSF1x::OpRun()   
//----------------------------------------------------------------------------
{
	albaString wildc = "MSF file (*.msf)|*.msf";
	albaString f;
  if(m_File.IsEmpty())
	{
		f = albaGetOpenFile(m_FileDir, wildc.GetCStr(), "Choose MSF file");
		m_File = f;
	}

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
	{
    wxSetWorkingDirectory(wxString(m_File.GetCStr()));
    ImportMSF();
	  result = OP_RUN_OK;
	}
	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterMSF1x::ImportMSF()
//----------------------------------------------------------------------------
{
  bool success = false;
//	wxBusyInfo wait("Loading file: ...");
  
  m_Importer = new albaMSFImporter;
  m_Importer->SetURL(m_File);
  m_Importer->SetRoot((albaVMERoot *)m_Input->GetRoot());
  
  //albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,preader));
  albaTagItem item;
  albaVMERoot *root = (albaVMERoot *)m_Input->GetRoot();
  root->GetTagArray()->GetTag("APP_STAMP" , item);

  success = (m_Importer->Restore() == ALBA_OK);

  if(!success)
    albaErrorMessage("I/O Error importing MSF file.");

  ((albaVMERoot *)m_Input->GetRoot())->GetStorage()->SetURL(m_File.GetCStr());
  ((albaVMERoot *)m_Input->GetRoot())->GetStorage()->ForceParserURL();
  ((albaVMERoot *)m_Input->GetRoot())->Update();

  root->GetTagArray()->SetTag("APP_STAMP", item.GetValue(), item.GetType());

  cppDEL(m_Importer);
}

//----------------------------------------------------------------------------
char ** albaOpImporterMSF1x::GetIcon()
{
#include "pic/MENU_IMPORT_MSF.xpm"
	return MENU_IMPORT_MSF_xpm;
}