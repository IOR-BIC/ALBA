/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMotionDataImporter.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 12:30:36 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoMotionDataImporter_H__
#define __mmoMotionDataImporter_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h" 
#include "mafOp.h"

#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVME.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;                      //era mflVME
class mafEvent;
//----------------------------------------------------------------------------
// mmoMotionDataImporter :
//----------------------------------------------------------------------------
/** */
template <class MotionReader>
class mmoMotionDataImporter : public mafOp
{
public:
  mmoMotionDataImporter(const wxString &label, const wxString &pgdWildc, const wxString &dicWildc) :  mafOp(label)
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
  ~mmoMotionDataImporter( ) 
    //----------------------------------------------------------------------------
  {
  }
  //----------------------------------------------------------------------------
  mafOp* Copy()   
    /** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
    //----------------------------------------------------------------------------
  {
    //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
    mmoMotionDataImporter *cp = new mmoMotionDataImporter(m_Label, m_pgdWildc, m_dicWildc);
    cp->m_Canundo = m_Canundo;
    cp->m_OpType = m_OpType;
    cp->m_Listener = m_Listener;
    cp->m_Next = NULL;

    cp->m_File = m_File;
    cp->m_Dict = m_Dict;
    cp->m_Vme = m_Vme;
    return cp;
  }

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) {return true;};

  //----------------------------------------------------------------------------
  /** Builds operation's interface. */
  void OpRun()   
    //----------------------------------------------------------------------------
  {

    int result = OP_RUN_CANCEL;
    m_File = "";
    m_Dict = "";

    mafString f = mafGetOpenFile(m_FileDir,m_pgdWildc).c_str(); 
    if(f != "")
    {
      m_File = f;
      f = mafGetOpenFile(m_DictDir,m_dicWildc,"Open Dictionary").c_str(); 
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
  /** Execute the operation. */
  void OpDo()   
    //----------------------------------------------------------------------------
  {
    assert(!m_Vme);

    //modified by Stefano. 18-9-2003
    wxBusyInfo wait("Please wait, working...");

    mafSmartPointer<MotionReader> reader;
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
  //** Makes the undo for the operation.
  void OpUndo()   

  //----------------------------------------------------------------------------
  {
  assert(m_Vme);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
  //m_Vme->Delete(); remove vme from the tree will kill it - we have not referenced it
  m_Vme = NULL;
  }
  */

protected:
  wxString m_FileDir;
	wxString m_DictDir;
	wxString m_File;
	wxString m_Dict;
  wxString m_pgdWildc;
  wxString m_dicWildc;
	mafVME  *m_Vme; 						// era mflVME *m_vme
	int m_DictionaryAvailable;
};

#endif
