/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-10 00:18:06 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorage.h"
#include "mafStorable.h"

//------------------------------------------------------------------------------
mafStorage::mafStorage()
//------------------------------------------------------------------------------
{
  m_Root        = NULL;
  m_RootElement = NULL;
}

//------------------------------------------------------------------------------
int mafStorage::Store()
//------------------------------------------------------------------------------
{
  int ret=InternalStore();
  m_ParserFileName=m_FileName; // set the new filename as current

  // here I should add a call for packing/sending files

  return ret;
}
//------------------------------------------------------------------------------
int mafStorage::Restore()
//------------------------------------------------------------------------------
{
  m_ParserFileName=m_FileName; // set the new filename as current
  return InternalRestore();
}

//------------------------------------------------------------------------------
void mafStorage::SetFileName(const char *name)
//------------------------------------------------------------------------------
{
  m_FileName.Copy(name); // force copying the const char reference
}

//------------------------------------------------------------------------------
const char *mafStorage::GetFileName()
//------------------------------------------------------------------------------
{
  return m_FileName;
}

//------------------------------------------------------------------------------
const char *mafStorage::GetPareserFileName()
//------------------------------------------------------------------------------
{
  return m_ParserFileName;
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