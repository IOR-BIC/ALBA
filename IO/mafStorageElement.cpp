/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorageElement.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-29 18:00:27 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorageElement.h"
#include "mafString.h"
#include "mafVector.txx"
#include <assert.h>

//------------------------------------------------------------------------------
mafStorageElement::mafStorageElement(mafStorageElement *parent,mafStorage *storage)
//------------------------------------------------------------------------------
{
  assert(storage); // no NULL storage is allowed
  m_Storage = storage;
  m_Parent = parent;
  //m_Children = new mafVector<mafStorageElement>;
  m_Children = NULL;
}
//------------------------------------------------------------------------------
mafStorageElement::~mafStorageElement()
//------------------------------------------------------------------------------
{
  // remove pointers...
  m_Storage = NULL;
  m_Parent = NULL;

  if (m_Children)
  {
    // remove all child nodes
    for (int i=0;i<m_Children->GetNumberOfItems();i++)
    {
      delete (*m_Children)[i];
    }
  
    cppDEL(m_Children);
  }  
}

//------------------------------------------------------------------------------
mafStorageElement *mafStorageElement::FindNestedElement(const char *name)
//------------------------------------------------------------------------------
{
  mafString node_name(name); // no memory copy, thanks mafString :-)

  // force children list creation
  mafVector<mafStorageElement *> *children=GetChildren();
  
  // to be rewritten as a map access
  for (int i=0;i<children->GetNumberOfItems();i++)
  {
    mafStorageElement *node=(*children)[i];
    if (node_name==node->GetName())
      return node;
  }

  return NULL;
}

