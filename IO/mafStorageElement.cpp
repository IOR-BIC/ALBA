/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorageElement.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-24 15:11:09 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorageElement.h"

//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(mafStorage *storage)
//------------------------------------------------------------------------------
{
  m_Storage=storage;
}
//------------------------------------------------------------------------------
mafStorageElement::~mafStorageElement()
//------------------------------------------------------------------------------
{
}
//------------------------------------------------------------------------------
mafStorage *mafStorageElement::GetStorage()
//------------------------------------------------------------------------------
{
  return m_Storage;
}

