/*=========================================================================

 Program: MAF2
 Module: mafStorage
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafStorage.h"
#include "mafStorable.h"
#include "mafDecl.h"

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafStorage);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafStorage::mafStorage()
//------------------------------------------------------------------------------
{
  m_Document        = NULL;
  m_DocumentElement = NULL;
  m_TmpFileId       = 0;
  m_ErrorCode       = 0;
	
  //Creating user app data directory if does not exist
  if(!wxDirExists(mafGetAppDataDirectory().c_str()))
    wxMkdir(mafGetAppDataDirectory().c_str());

  //Creating user app tmp directory if does not exist
	m_TmpFolder=(mafGetAppDataDirectory()+"\\tmp").c_str();

	if(!wxDirExists(m_TmpFolder))
		wxMkdir(m_TmpFolder.GetCStr());
  m_NeedsUpgrade    = false;
}

//------------------------------------------------------------------------------
int mafStorage::Store()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  // extract the path substring
  mafString dir_path= wxPathOnly(m_URL.GetCStr());
  
  //open the directory index
  if (OpenDirectory(dir_path)==MAF_ERROR)
  {
    mafErrorMessage("I/O Error: store failed because path not found!");
    return MAF_ERROR;
  }

  // store the content
  int ret=InternalStore();
  
  // set the new filename as current
  m_ParserURL=m_URL; 

  // here I should add a call for packing/sending files
  
  return ret;
}
//------------------------------------------------------------------------------
int mafStorage::Restore()
//------------------------------------------------------------------------------
{
  SetErrorCode(0);
  m_ParserURL = m_URL; // set the new filename as current
  return InternalRestore();
}

//------------------------------------------------------------------------------
void mafStorage::SetURL(const char *name)
//------------------------------------------------------------------------------
{
  m_URL=name; // force copying the const char reference
}

//------------------------------------------------------------------------------
void mafStorage::ForceParserURL()
//------------------------------------------------------------------------------
{
  m_ParserURL = m_URL; // set the new filename as current
}

//------------------------------------------------------------------------------
const char *mafStorage::GetURL()
//------------------------------------------------------------------------------
{
  return m_URL;
}

//------------------------------------------------------------------------------
const char *mafStorage::GetPareserURL()
//------------------------------------------------------------------------------
{
  return m_ParserURL;
}

//----------------------------------------------------------------------------
bool mafStorage::IsFileInDirectory(const char *filename)
//----------------------------------------------------------------------------
{
  return m_FilesDictionary.find(filename)!=m_FilesDictionary.end();
}

//------------------------------------------------------------------------------
void mafStorage::SetDocument(mafStorable *doc)
//------------------------------------------------------------------------------
{
  m_Document=doc;
}
//------------------------------------------------------------------------------
mafStorable *mafStorage::GetDocument()
//------------------------------------------------------------------------------
{
  return m_Document;
}

//------------------------------------------------------------------------------
void mafStorage::GetTmpFile(mafString &filename)
//------------------------------------------------------------------------------
{
  mafString tmpfname=GetTmpFolder();
  tmpfname<<"#tmp.";
  do 
  {
    tmpfname<<mafString(m_TmpFileId++);	
  } while(m_TmpFileNames.find(tmpfname)!=m_TmpFileNames.end());
  
  filename=tmpfname;
  m_TmpFileNames.insert(filename);
}
//------------------------------------------------------------------------------
void mafStorage::ReleaseTmpFile(const char *filename)
//------------------------------------------------------------------------------
{
  std::set<mafString>::iterator it=m_TmpFileNames.find(filename);
  if (it!=m_TmpFileNames.end())
  {
    m_TmpFileNames.erase(it);
  }

#ifdef MAF_USE_WX
  // remove file from disk if present
  wxRemoveFile(filename);
#endif

}
