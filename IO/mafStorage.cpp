/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-29 18:00:27 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorage.h"

//------------------------------------------------------------------------------
mafStorage::mafStorage()
//------------------------------------------------------------------------------
{
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