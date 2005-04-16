/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-16 12:09:05 $
  Version:   $Revision: 1.10 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorage.h"
#include "mafStorable.h"

//------------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafStorage);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafStorage::mafStorage()
//------------------------------------------------------------------------------
{
  m_Document        = NULL;
  m_DocumentElement = NULL;
  m_TmpFileId   = 0;
}

//------------------------------------------------------------------------------
int mafStorage::Store()
//------------------------------------------------------------------------------
{
  int ret=InternalStore();
  m_ParserURL=m_URL; // set the new filename as current

  // here I should add a call for packing/sending files

  return ret;
}
//------------------------------------------------------------------------------
int mafStorage::Restore()
//------------------------------------------------------------------------------
{
  m_ParserURL=m_URL; // set the new filename as current
  return InternalRestore();
}

//------------------------------------------------------------------------------
void mafStorage::SetURL(const char *name)
//------------------------------------------------------------------------------
{
  m_URL=name; // force copying the const char reference
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
  mafString tmpfname="~tmp.";
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
