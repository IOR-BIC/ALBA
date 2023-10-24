/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterExternalFile
 Authors: Paolo Quadrani     Stefano Perticoni   Roberto Mucci
 
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

#include "albaOpImporterExternalFile.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaVMEExternalData.h"
#include "wx/filename.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterExternalFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterExternalFile::albaOpImporterExternalFile(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Input = NULL; 

  m_Vme = NULL;
	m_FileDir = albaGetLastUserFolder();
}
//----------------------------------------------------------------------------
albaOpImporterExternalFile::~albaOpImporterExternalFile( ) 
//----------------------------------------------------------------------------
{
  albaDEL(m_Vme);
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterExternalFile::Copy()   
//----------------------------------------------------------------------------
{
  albaOpImporterExternalFile *cp =  new albaOpImporterExternalFile(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterExternalFile::OpRun()   
//----------------------------------------------------------------------------
{
  albaString wildc = _("All Files (*.*) |*.*");
  albaString f;
  if (m_File.IsEmpty())
  {
    f = albaGetOpenFile(m_FileDir.GetCStr(),wildc.GetCStr());
    m_File = f;
  }
  
  int result = OP_RUN_CANCEL;

  if(!m_File.IsEmpty()) 
  {
    ImportExternalFile();
    result = OP_RUN_OK;
  }
  albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
void albaOpImporterExternalFile::ImportExternalFile()
//----------------------------------------------------------------------------
{
	wxString path, name, ext;
	wxFileName::SplitPath(m_File.GetCStr(), &path, &name, &ext);
  
	wxString vmeName;
	vmeName << name << "." << ext;

  albaNEW(m_Vme);
	m_Vme->SetExtension(ext);
	m_Vme->SetFileName(name.char_str());
  m_Vme->SetCurrentPath(path.char_str());
	m_Vme->SetName(vmeName.char_str());
  m_Vme->ReparentTo(m_Input);
  m_Vme->Update();

  m_Output = m_Vme;
}

//----------------------------------------------------------------------------
void albaOpImporterExternalFile::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_Vme);
	GetLogicManager()->VmeRemove(m_Vme);
}
