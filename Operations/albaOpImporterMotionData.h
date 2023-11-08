/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMotionData
 Authors: Fedor Moiseev, Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpImporterMotionData_H__
#define __albaOpImporterMotionData_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h" 
#include "albaOp.h"

#include <wx/busyinfo.h>

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaVME.h"
#include "albaTagArray.h"
#include "albaSmartPointer.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;                      
class albaEvent;
//----------------------------------------------------------------------------
// albaOpImporterMotionData :
//----------------------------------------------------------------------------
/** */
template <class MotionReader>
class albaOpImporterMotionData : public albaOp
{
public:
//----------------------------------------------------------------------------
  albaOpImporterMotionData(wxString label, wxString pgdWildc, wxString dicWildc) :  albaOp(label), m_PgdWildc(pgdWildc), m_DicWildc(dicWildc)
  
  {
    m_OpType	= OPTYPE_IMPORTER;
    m_Canundo	= true;
    m_File		= "";
    m_Dict		= "";
    m_FileDir = (albaGetApplicationDirectory() + "/Data/External/").ToAscii();
    m_DictDir = (albaGetApplicationDirectory() + "/Config/Dictionary/").ToAscii();

    m_Vme		= NULL;

    m_DictionaryAvailable = 0;
  }
//----------------------------------------------------------------------------
  ~albaOpImporterMotionData( ) 
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
  albaOp* Copy()   
  {
    albaOpImporterMotionData *cp = new albaOpImporterMotionData(m_Label, m_PgdWildc, m_DicWildc);
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
/** Builds operation's interface. */
  void OpRun()   
  {
    int result = OP_RUN_CANCEL;
    m_File = "";
    m_Dict = "";

    albaString f = albaGetOpenFile(m_FileDir,m_PgdWildc); 
    if(f != "")
    {
      m_File = f;
      f = albaGetOpenFile(m_DictDir,m_DicWildc,"Open Dictionary"); 
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
    albaEventMacro(albaEvent(this,result));
  }
//----------------------------------------------------------------------------
/** Execute the operation. */
  void OpDo()   
  {
    // Modified by Simone Brazzale, 03/12/2010
    assert(!m_Vme);

    if (!m_TestMode)
    {
      wxBusyInfo wait("Please wait, working...");
    }

    albaSmartPointer<MotionReader> reader; 
    reader->SetFileName(m_File);
    reader->SetDictionaryFileName(m_Dict);

    if (GetDictionaryFlag()==1)
      reader->DictionaryOn();
    else
      reader->DictionaryOff();

    reader->Read();

    m_Vme = reader;

    wxString path, name, ext;
    wxFileName::SplitPath(m_File,&path,&name,&ext);
    m_Vme->SetName(name);

    albaTagItem tag_Nature;
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
  {
    m_File = filename;
  }
//----------------------------------------------------------------------------
/** Set dictionary name. */
  void SetDictionaryName(wxString dicname)   
  {
    m_Dict = dicname;
  }
//----------------------------------------------------------------------------
/** Set dictionary flag. */
  void SetDictionaryFlagOn()   
  {
    this->m_DictionaryAvailable = 1;
  }
//----------------------------------------------------------------------------
/** Set dictionary flag. */
  void SetDictionaryFlagOff()   
  {
    this->m_DictionaryAvailable = 0;
  }
//----------------------------------------------------------------------------
/** Get dictionary flag. */
  int GetDictionaryFlag()   
  {
    return this->m_DictionaryAvailable;
  }
//----------------------------------------------------------------------------
/** Get output. */
  albaVME* GetOutput()   
  {
    return this->m_Vme;
  }
//----------------------------------------------------------------------------
//** Makes the undo for the operation.
  void OpUndo()   
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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) {	return true; };

  wxString m_FileDir;
	wxString m_DictDir;
	wxString m_File;
	wxString m_Dict;
  wxString m_PgdWildc;
  wxString m_DicWildc;
	albaVME  *m_Vme; 						
	int m_DictionaryAvailable;
};

#endif
