/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-28 19:45:25 $
  Version:   $Revision: 1.2 $
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
void int::Store()
//------------------------------------------------------------------------------
{
  int ret=InternalStore();
  m_ParserFileName=FileName; // set the new filename as current

  // here I should add a call for packing/sending files

  return ret;
}
//------------------------------------------------------------------------------
int mafStorage::Restore()
//------------------------------------------------------------------------------
{
  m_ParserFileName=FileName; // set the new filename as current
  return InternalRestore();
}

//------------------------------------------------------------------------------
void mafStorage::SetFileName(const char *name)
//------------------------------------------------------------------------------
{
  FileName.Copy(name); // force copying the const char reference
}

//------------------------------------------------------------------------------
const char *mafStorage::GetFileName()
//------------------------------------------------------------------------------
{
  return FileName;
}

//------------------------------------------------------------------------------
const char *mafStorage::GetPareserFileName()
//------------------------------------------------------------------------------
{
  return m_ParserFileName;
}