/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:18:10 $
  Version:   $Revision: 1.9 $
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
  m_Root        = NULL;
  m_RootElement = NULL;
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
void mafStorage::SetRoot(mafStorable *root)
//------------------------------------------------------------------------------
{
  m_Root=root;
}
//------------------------------------------------------------------------------
mafStorable *mafStorage::GetRoot()
//------------------------------------------------------------------------------
{
  return m_Root;
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
  // file deletion not yet implemented
}
