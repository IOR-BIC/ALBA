/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaStorage
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaStorage.h"
#include "albaStorable.h"
#include "albaDecl.h"

//------------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaStorage);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaStorage::albaStorage()
//------------------------------------------------------------------------------
{
  m_Document        = NULL;
  m_DocumentElement = NULL;
  m_TmpFileId       = 0;
  m_ErrorCode       = 0;
	
  //Creating user app data directory if does not exist
  if(!wxDirExists(albaGetAppDataDirectory().ToAscii()))
    wxMkdir(albaGetAppDataDirectory().ToAscii());

  //Creating user app tmp directory if does not exist
	m_TmpFolder=(albaGetAppDataDirectory()+"\\tmp");

	if(!wxDirExists(m_TmpFolder.GetCStr()))
		wxMkdir(m_TmpFolder.GetCStr());
  m_NeedsUpgrade    = false;
}

//------------------------------------------------------------------------------
int albaStorage::Store()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  // extract the path substring
  albaString dir_path= wxPathOnly(m_URL.GetCStr());
  
  //open the directory index
  if (OpenDirectory(dir_path)==ALBA_ERROR)
  {
    albaErrorMessage("I/O Error: store failed because path not found!");
    return ALBA_ERROR;
  }

  // store the content
  int ret=InternalStore();
  
  // set the new filename as current
  m_ParserURL=m_URL; 

  // here I should add a call for packing/sending files
  
  return ret;
}
//------------------------------------------------------------------------------
int albaStorage::Restore()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  m_ParserURL = m_URL; // set the new filename as current
  return InternalRestore();
}

//------------------------------------------------------------------------------
void albaStorage::SetURL(const char *name)
//------------------------------------------------------------------------------
{
  m_URL=name; // force copying the const char reference
}

//------------------------------------------------------------------------------
void albaStorage::ForceParserURL()
//------------------------------------------------------------------------------
{
  m_ParserURL = m_URL; // set the new filename as current
}

//------------------------------------------------------------------------------
const char *albaStorage::GetURL()
//------------------------------------------------------------------------------
{
  return m_URL;
}

//------------------------------------------------------------------------------
const char *albaStorage::GetPareserURL()
//------------------------------------------------------------------------------
{
  return m_ParserURL;
}

//----------------------------------------------------------------------------
bool albaStorage::IsFileInDirectory(const char *filename)
//----------------------------------------------------------------------------
{
  return m_FilesDictionary.find(filename)!=m_FilesDictionary.end();
}

//------------------------------------------------------------------------------
void albaStorage::SetDocument(albaStorable *doc)
//------------------------------------------------------------------------------
{
  m_Document=doc;
}
//------------------------------------------------------------------------------
albaStorable *albaStorage::GetDocument()
//------------------------------------------------------------------------------
{
  return m_Document;
}

//------------------------------------------------------------------------------
void albaStorage::GetTmpFile(albaString &filename)
//------------------------------------------------------------------------------
{
  albaString tmpfname=GetTmpFolder();
  tmpfname<<"#tmp.";
  do 
  {
    tmpfname<<albaString(m_TmpFileId++);	
  } while(m_TmpFileNames.find(tmpfname)!=m_TmpFileNames.end());
  
  filename=tmpfname;
  m_TmpFileNames.insert(filename);
}
//------------------------------------------------------------------------------
void albaStorage::ReleaseTmpFile(const char *filename)
//------------------------------------------------------------------------------
{
  std::set<albaString>::iterator it=m_TmpFileNames.find(filename);
  if (it!=m_TmpFileNames.end())
  {
    m_TmpFileNames.erase(it);
  }

#ifdef ALBA_USE_WX
  // remove file from disk if present
  wxRemoveFile(filename);
#endif

}
