/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpOpenExternalFile.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-18 15:50:56 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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
bool mafOpOpenExternalFile::Accept(mafNode *node)
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
		m_Pid = wxExecute(command2execute, FALSE);
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
