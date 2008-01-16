/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpImporterExternalFile.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-16 11:05:22 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani     Stefano Perticoni   Roberto Mucci
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

#include "mafOpImporterExternalFile.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEExternalData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterExternalFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterExternalFile::mafOpImporterExternalFile(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Input = NULL; 

  m_Vme = NULL;
	m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterExternalFile::~mafOpImporterExternalFile( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Vme);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterExternalFile::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterExternalFile *cp =  new mafOpImporterExternalFile(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterExternalFile::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = _("All Files (*.*) |*.*");
  mafString f;
  if (m_File.IsEmpty())
  {
    f = mafGetOpenFile(m_FileDir.GetCStr(),wildc.GetCStr()).c_str();
    m_File = f;
  }
  
  int result = OP_RUN_CANCEL;

  if(!m_File.IsEmpty()) 
  {
    ImportExternalFile();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void mafOpImporterExternalFile::ImportExternalFile()
//----------------------------------------------------------------------------
{
	wxString path, name, ext;
	wxSplitPath(m_File, &path, &name, &ext);
  
	wxString vmeName;
	vmeName << name << "." << ext;

  mafNEW(m_Vme);
	m_Vme->SetExtension(ext);
	m_Vme->SetFileName(name.c_str());
  m_Vme->SetCurrentPath(path.c_str());
	m_Vme->SetName(vmeName.c_str());
  m_Vme->Update();

  m_Output = m_Vme;
}

//----------------------------------------------------------------------------
void mafOpImporterExternalFile::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_Vme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
}
