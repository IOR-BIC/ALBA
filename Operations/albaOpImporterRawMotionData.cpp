/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)Medical
 Module: albaOpImporterRawMotionData
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

#include "albaOpImporterRawMotionData.h"
#include <wx/busyinfo.h>


#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVME.h"
#include "albaVMERawMotionData.h" // vedere
#include "albaTagArray.h"
#include "albaSmartPointer.h"


// #include <mflVMEIterator.h> non ce ne e' bisogno

//----------------------------------------------------------------------------
albaOpImporterRawMotionData::albaOpImporterRawMotionData(wxString label) :
albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= false;
	m_File		= "";
	m_Dict		= "";
	m_FileDir = (albaGetApplicationDirectory() + "/Data/External/").c_str();
	m_DictDir = (albaGetApplicationDirectory() + "/Config/Dictionary/").c_str();

	m_Vme		= NULL;
	
	m_DictionaryAvailable = 0;
}
//----------------------------------------------------------------------------
albaOpImporterRawMotionData::~albaOpImporterRawMotionData( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterRawMotionData::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
	//non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
	albaOpImporterRawMotionData *cp = new albaOpImporterRawMotionData(m_Label);
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
void albaOpImporterRawMotionData::OpRun()   
//----------------------------------------------------------------------------
{
  
	int result = OP_RUN_CANCEL;
	m_File = "";
	m_Dict = "";
	wxString pgd_wildc	= "RAW Motion Data (*.MAN)|*.MAN";
	wxString dict_wildc = "Dictionary (*.txt)|*.txt";

	albaString f = albaGetOpenFile(m_FileDir,pgd_wildc).c_str(); 
	if(f != "")
	{
	m_File = f;
	f = albaGetOpenFile(m_DictDir,dict_wildc,"Open Dictionary").c_str(); 
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
	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterRawMotionData::OpDo()   
//----------------------------------------------------------------------------
{
	assert(!m_Vme);
	
	//modified by Stefano. 18-9-2003
	wxBusyInfo wait("Please wait, working...");

	albaSmartPointer<albaVMERawMotionData> reader;
	reader->SetFileName(m_File);
	reader->SetDictionaryFileName(m_Dict);

	if (this->m_DictionaryAvailable)
		reader->DictionaryOn();
	else
		reader->DictionaryOff();

	reader->Read();

	m_Vme = reader;

  wxString path, name, ext;
  wxFileName::SplitPath(m_File.c_str(),&path,&name,&ext);
  m_Vme->SetName(name);
	
	albaTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_Vme->GetTagArray()->SetTag(tag_Nature); //m_Vme->GetTagArray()->AddTag(tag_Nature);

	GetLogicManager()->VmeAdd(m_Vme);
}
/*
//----------------------------------------------------------------------------
void albaOpImporterRawMotionData::OpUndo()   

//----------------------------------------------------------------------------
{
	assert(m_Vme);
	albaEventMacro(albaEvent(this,VME_REMOVE,m_Vme));
	//m_Vme->Delete(); remove vme from the tree will kill it - we have not referenced it
	m_Vme = NULL;
}
*/
