/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpImporterRawMotionData.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-28 08:47:16 $
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

#include "medOpImporterRawMotionData.h"
#include <wx/busyinfo.h>


#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVMERawMotionData.h" // vedere
#include "mafTagArray.h"
#include "mafSmartPointer.h"


// #include <mflVMEIterator.h> non ce ne e' bisogno

//----------------------------------------------------------------------------
medOpImporterRawMotionData::medOpImporterRawMotionData(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= false;
	m_File		= "";
	m_Dict		= "";
	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
	m_DictDir = (mafGetApplicationDirectory() + "/Config/Dictionary/").c_str();

	m_Vme		= NULL;
	
	m_DictionaryAvailable = 0;
}
//----------------------------------------------------------------------------
medOpImporterRawMotionData::~medOpImporterRawMotionData( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpImporterRawMotionData::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
	//non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
	medOpImporterRawMotionData *cp = new medOpImporterRawMotionData(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_Dict = m_Dict;
	cp->m_Vme = m_Vme;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterRawMotionData::OpRun()   
//----------------------------------------------------------------------------
{
  
	int result = OP_RUN_CANCEL;
	m_File = "";
	m_Dict = "";
	wxString pgd_wildc	= "RAW Motion Data (*.MAN)|*.MAN";
	wxString dict_wildc = "Dictionary (*.txt)|*.txt";

	mafString f = mafGetOpenFile(m_FileDir,pgd_wildc).c_str(); 
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
void medOpImporterRawMotionData::OpDo()   
//----------------------------------------------------------------------------
{
	assert(!m_Vme);
	
	//modified by Stefano. 18-9-2003
	wxBusyInfo wait("Please wait, working...");

	mafSmartPointer<mafVMERawMotionData> reader;
	reader->SetFileName(m_File);
	reader->SetDictionaryFileName(m_Dict);

	if (this->m_DictionaryAvailable)
		reader->DictionaryOn();
	else
		reader->DictionaryOff();

	reader->Read();

	m_Vme = reader;

  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);
  m_Vme->SetName(name);
	
	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_Vme->GetTagArray()->SetTag(tag_Nature); //m_Vme->GetTagArray()->AddTag(tag_Nature);

	mafEventMacro(mafEvent(this,VME_ADD,m_Vme));
}
/*
//----------------------------------------------------------------------------
void medOpImporterRawMotionData::OpUndo()   

//----------------------------------------------------------------------------
{
	assert(m_Vme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
	//m_Vme->Delete(); remove vme from the tree will kill it - we have not referenced it
	m_Vme = NULL;
}
*/
