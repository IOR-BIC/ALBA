/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorageElement.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-28 19:45:26 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorageElement.h"
#include <assert.h>

//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(mafStorageElement *parent,mafStorage *storage)
//------------------------------------------------------------------------------
{
  assert(storage); // no NULL storage is allowed
  m_Storage=storage;
  m_Parent=parent;
  m_Children = new mafVector<mafStorageElement>;
}
//------------------------------------------------------------------------------
mafStorageElement::~mafStorageElement()
//------------------------------------------------------------------------------
{
  // remove pointers...
  m_Storage = NULL;
  m_Parent = NULL;

  // remove all child nodes
  for (int i=0;i<m_Children->GetNumberOfItems();i++)
  {
    delete m_Children[i];
  }
  
  cppDEL(m_Children);
}

