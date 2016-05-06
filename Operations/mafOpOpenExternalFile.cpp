/*=========================================================================

 Program: MAF2
 Module: mafOpOpenExternalFile
 Authors: Stefano Perticoni   Roberto Mucci
 
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
// This force to include Window, wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpOpenExternalFile.h"

#include <wx/mimetype.h>

#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEExternalData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpOpenExternalFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpOpenExternalFile::mafOpOpenExternalFile(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = false;
  m_Input		= NULL; 
}
//----------------------------------------------------------------------------
mafOpOpenExternalFile::~mafOpOpenExternalFile( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpOpenExternalFile::Copy()   
//----------------------------------------------------------------------------
{
  return (new mafOpOpenExternalFile(m_Label));
}

//----------------------------------------------------------------------------
bool mafOpOpenExternalFile::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  return (node != NULL && node->IsMAFType(mafVMEExternalData));
}
//----------------------------------------------------------------------------
void mafOpOpenExternalFile::OpRun()   
//----------------------------------------------------------------------------
{
	wxString file, ext, mime, command2execute;

	file = ((mafVMEExternalData *)(this->m_Input))->GetAbsoluteFileName().GetCStr();
  
  ext = ((mafVMEExternalData *)(this->m_Input))->GetExtension();

	wxFileType *filetype;
	filetype = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
	if (filetype == NULL)
  {
		wxMessageBox("Don't know how to handle this type of file");
		mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
		return;
	}
	
	filetype->GetMimeType(&mime);

	if (filetype->GetOpenCommand(&command2execute, wxFileType::MessageParameters(file, mime)))
	{
    command2execute.Replace("/", "\\");
    mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );
		m_Pid = wxExecute(command2execute); //,FALSE
		cppDEL(filetype);
		mafEventMacro(mafEvent(this,OP_RUN_OK));
	}
	else
	{
		wxMessageBox("Don't know how to handle this type of file");
		cppDEL(filetype);
		mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
	}	
}
//----------------------------------------------------------------------------
long mafOpOpenExternalFile::GetPid()   
//----------------------------------------------------------------------------
{
  return m_Pid;
}
