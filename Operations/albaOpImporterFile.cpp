/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporter
 Authors: Gianluigi Crimi
 
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

#include "albaOpImporterFile.h"


//----------------------------------------------------------------------------
albaOpImporterFile::albaOpImporterFile(const wxString &label) :
albaOp(label)
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_FileName = "";
}

//----------------------------------------------------------------------------
albaOpImporterFile::~albaOpImporterFile()
{
}

//----------------------------------------------------------------------------
void albaOpImporterFile::SetFileName(albaString fileName)
{
	m_FileName = fileName;
}

//----------------------------------------------------------------------------
int albaOpImporterFile::AcceptFile(albaString fileName)
{
		wxString file(fileName.GetCStr());
		wxString ext = file.AfterLast('.');
					
		wxString wildcards(m_Wildc.GetCStr());
		wxArrayString patterns = wxSplit(wildcards, ';');
					
		for (size_t i = 0; i < patterns.GetCount(); ++i)
		{
			wxString pattern = patterns[i].AfterLast('.');
			if (pattern.CmpNoCase(ext) == 0)
				return true;
		}
					
		return false;  
}

//----------------------------------------------------------------------------
void albaOpImporterFile::SetWildc(albaString wildc)
{
	m_Wildc = wildc;
}

