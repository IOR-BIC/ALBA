/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMSFImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-07 17:27:05 $
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

#include "mmoMSFImporter.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"

#include "mafVME.h"
#include "mafTagArray.h"
#include "mafMSFImporter.h"

//----------------------------------------------------------------------------
mmoMSFImporter::mmoMSFImporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoMSFImporter::~mmoMSFImporter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mmoMSFImporter::Copy()   
//----------------------------------------------------------------------------
{
  //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
  mmoMSFImporter *cp = new mmoMSFImporter(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mmoMSFImporter::OpRun()   
//----------------------------------------------------------------------------
{
	m_File = "";

	wxString wildc = "MSF file (*.msf)|*.msf";
  wxString f = mafGetOpenFile(m_FileDir, wildc, "Choose MSF file").c_str();

  int result = OP_RUN_CANCEL;
  if(f != "") 
	{
	  m_File = f;
    ImportMSF();
	  result = OP_RUN_OK;
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoMSFImporter::ImportMSF()
//----------------------------------------------------------------------------
{
  bool success = false;
	wxBusyInfo wait("Loading file: ...");
  
  m_Importer = new mafMSFImporter;
  m_Importer->SetURL(m_File);
  
  //mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,preader));
  
  success = (m_Importer->Restore() == MAF_OK);

  if(!success)
    mafErrorMessage("I/O Error importing MSF file.");

  cppDEL(m_Importer);
}
