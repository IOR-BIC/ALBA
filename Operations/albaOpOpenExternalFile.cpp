/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpOpenExternalFile
 Authors: Stefano Perticoni   Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpOpenExternalFile.h"

#include <wx/mimetype.h>

#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMEExternalData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpOpenExternalFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpOpenExternalFile::albaOpOpenExternalFile(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = false;
  m_Input		= NULL; 
}
//----------------------------------------------------------------------------
albaOpOpenExternalFile::~albaOpOpenExternalFile( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpOpenExternalFile::Copy()   
//----------------------------------------------------------------------------
{
  return (new albaOpOpenExternalFile(m_Label));
}

//----------------------------------------------------------------------------
bool albaOpOpenExternalFile::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node != NULL && node->IsALBAType(albaVMEExternalData));
}
//----------------------------------------------------------------------------
void albaOpOpenExternalFile::OpRun()   
//----------------------------------------------------------------------------
{
	wxString file, ext, mime, command2execute;

	file = ((albaVMEExternalData *)(this->m_Input))->GetAbsoluteFileName().GetCStr();
  
  ext = ((albaVMEExternalData *)(this->m_Input))->GetExtension();

	wxFileType *filetype;
	filetype = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
	if (filetype == NULL)
  {
		wxMessageBox("Don't know how to handle this type of file");
		albaEventMacro(albaEvent(this,OP_RUN_CANCEL));
		return;
	}
	
	filetype->GetMimeType(&mime);

	if (filetype->GetOpenCommand(&command2execute, wxFileType::MessageParameters(file, mime)))
	{
    command2execute.Replace("/", "\\");
    albaLogMessage("Executing command: '%s'", command2execute.ToAscii());
		m_Pid = wxExecute(command2execute); //,FALSE
		cppDEL(filetype);
		albaEventMacro(albaEvent(this,OP_RUN_OK));
	}
	else
	{
		wxMessageBox("Don't know how to handle this type of file");
		cppDEL(filetype);
		albaEventMacro(albaEvent(this,OP_RUN_CANCEL));
	}	
}
//----------------------------------------------------------------------------
long albaOpOpenExternalFile::GetPid()   
//----------------------------------------------------------------------------
{
  return m_Pid;
}
