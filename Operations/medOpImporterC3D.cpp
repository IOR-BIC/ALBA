/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterC3D.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:38:31 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani - porting Daniele Giunchi
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
#include "medOpImporterC3D.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVME.h"
//#include "mafC3DReader.h"

//----------------------------------------------------------------------------
medOpImporterC3D::medOpImporterC3D(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File		= "";
	m_Vme			= NULL;
	this->m_DictionaryAvailable = 0;

	m_FileDir = mafGetApplicationDirectory().c_str(); 
  m_FileDir +=  "/Data/External/";
	m_DictDir = mafGetApplicationDirectory().c_str();
  m_DictDir += "/Config/Dictionary/";
}
//----------------------------------------------------------------------------
medOpImporterC3D::~medOpImporterC3D( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpImporterC3D::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
	//non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
	medOpImporterC3D *cp = new medOpImporterC3D(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_Vme = m_Vme;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterC3D::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = "";
	m_Dict = "";
	wxString c3d_wildc	= "C3D Data (*.c3d)|*.c3d";
	wxString dict_wildc = "Dictionary (*.txt)|*.txt";

	wxString f = mafGetOpenFile(m_FileDir,c3d_wildc).c_str(); 
	if(f != "")
	{
		m_File = f;
		f = mafGetOpenFile(m_DictDir,dict_wildc,"Open Dictionary").c_str(); 
		if(f != "")
		{
			m_Dict = f;
			this->m_DictionaryAvailable = 1;
			result = OP_RUN_OK;
		}
		else
		{
			this->m_DictionaryAvailable = 0;
			result = OP_RUN_OK;
		}
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpImporterC3D::OpDo()   
/**  */
//----------------------------------------------------------------------------
{
  assert(!m_Vme);
	
	//modified by Stefano. 18-9-2003
	wxBusyInfo wait("Please wait, working...");
  
  //mafC3DReader *reader = mafC3DReader::New();
  //reader->SetFileName(m_File);
	//reader->SetDictionaryFileName(m_Dict);

	if (this->m_DictionaryAvailable)
		;//reader->DictionaryOn();
	else
		;//reader->DictionaryOff();

	//reader->Read();

  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);

	//m_vme = reader->GetOutput()->GetFirstChild();
	//m_Vme = reader->GetOutput();
  m_Vme->SetName(name);

	//mafEventMacro(mafEvent(this,VME_ADD,m_Vme));
	//reader->Delete();
}
//----------------------------------------------------------------------------
void medOpImporterC3D::OpUndo()   
/**  */
//----------------------------------------------------------------------------
{
	assert(m_Vme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
	m_Vme->Delete();
	m_Vme = NULL;
}
