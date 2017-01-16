/*=========================================================================

 Program: MAF2
 Module: mafOpImporterMotionData
 Authors: Fedor Moiseev, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpImporterMotionData_H__
#define __mafOpImporterMotionData_H__

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
class mafVME;                      
class mafEvent;
//----------------------------------------------------------------------------
// mafOpImporterMotionData :
//----------------------------------------------------------------------------
/** */
template <class MotionReader>
class mafOpImporterMotionData : public mafOp
{
public:
  mafOpImporterMotionData(const wxString &label, const wxString &pgdWildc, const wxString &dicWildc) :  mafOp(label), m_PgdWildc(pgdWildc), m_DicWildc(dicWildc)
        //----------------------------------------------------------------------------
  {
    m_OpType	= OPTYPE_IMPORTER;
    m_Canundo	= true;
    m_File		= "";
    m_Dict		= "";
    m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();
    m_DictDir = (mafGetApplicationDirectory() + "/Config/Dictionary/").c_str();

    m_Vme		= NULL;

    m_DictionaryAvailable = 0;
  }
      //----------------------------------------------------------------------------
  ~mafOpImporterMotionData( ) 
    //----------------------------------------------------------------------------
  {
    // Must unregister in order to avoid leaks or data loss
    if (m_Output)
    {
      m_Vme->UnRegister(m_Output);
      m_Output = NULL;
    }
    m_Vme = NULL;    
  }
  //----------------------------------------------------------------------------
  mafOp* Copy()   
    /** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
    //----------------------------------------------------------------------------
  {
    //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
    mafOpImporterMotionData *cp = new mafOpImporterMotionData(m_Label, m_PgdWildc, m_DicWildc);
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
	bool Accept(mafVME* node) {return true;};

  //----------------------------------------------------------------------------
  /** Builds operation's interface. */
  void OpRun()   
    //----------------------------------------------------------------------------
  {
    int result = OP_RUN_CANCEL;
    m_File = "";
    m_Dict = "";

    mafString f = mafGetOpenFile(m_FileDir,m_PgdWildc).c_str(); 
    if(f != "")
    {
      m_File = f;
      f = mafGetOpenFile(m_DictDir,m_DicWildc,"Open Dictionary").c_str(); 
      if(f != "")
      {
        m_Dict = f;
        SetDictionaryFlagOn();
        result = OP_RUN_OK;
      }
      else
      {
        SetDictionaryFlagOff();
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
    // Modified by Simone Brazzale, 03/12/2010
    assert(!m_Vme);

    if (!m_TestMode)
    {
      wxBusyInfo wait("Please wait, working...");
    }

    mafSmartPointer<MotionReader> reader; 
    reader->SetFileName(m_File);
    reader->SetDictionaryFileName(m_Dict);

    if (GetDictionaryFlag()==1)
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

    m_Vme->GetTagArray()->SetTag(tag_Nature); 

    // Must register in order to preserve output for do/undo operation (since it is a smart pointer)
    m_Output = m_Vme;
    m_Vme->Register(m_Output);
    GetLogicManager()->VmeAdd(m_Vme);
  }
  //----------------------------------------------------------------------------
  /** Set file name. */
  void SetFileName(wxString filename)   
    //----------------------------------------------------------------------------
  {
    m_File = filename;
  }
  //----------------------------------------------------------------------------
  /** Set dictionary name. */
  void SetDictionaryName(wxString dicname)   
    //----------------------------------------------------------------------------
  {
    m_Dict = dicname;
  }
  //----------------------------------------------------------------------------
  /** Set dictionary flag. */
  void SetDictionaryFlagOn()   
    //----------------------------------------------------------------------------
  {
    this->m_DictionaryAvailable = 1;
  }
  //----------------------------------------------------------------------------
  /** Set dictionary flag. */
  void SetDictionaryFlagOff()   
    //----------------------------------------------------------------------------
  {
    this->m_DictionaryAvailable = 0;
  }
  //----------------------------------------------------------------------------
  /** Get dictionary flag. */
  int GetDictionaryFlag()   
    //----------------------------------------------------------------------------
  {
    return this->m_DictionaryAvailable;
  }
  //----------------------------------------------------------------------------
  /** Get output. */
  mafVME* GetOutput()   
    //----------------------------------------------------------------------------
  {
    return this->m_Vme;
  }
  //----------------------------------------------------------------------------
  //** Makes the undo for the operation.
  void OpUndo()   
  //----------------------------------------------------------------------------
  {
    assert(m_Vme);
    m_Output = NULL;
    // Must unregister in order to delete completely all data (since it was a smart pointer)
    m_Vme->UnRegister(m_Output);
    m_Vme->ReparentTo(NULL);
    // m_Vme->Delete(); // remove vme from the tree will kill it - do not use this if it has been previously registered
    m_Vme = NULL;
  }
  

protected:
  wxString m_FileDir;
	wxString m_DictDir;
	wxString m_File;
	wxString m_Dict;
  wxString m_PgdWildc;
  wxString m_DicWildc;
	mafVME  *m_Vme; 						
	int m_DictionaryAvailable;
};

#endif
