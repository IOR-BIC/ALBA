/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMSFStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-16 12:08:36 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafIncludeWX.h" // to be removed

#include "mafMSFStorage.h"
#include "mafVMERoot.h"
#include "mmuUtility.h"
#include "mafStorable.h"
#include "mafStorageElement.h"

//------------------------------------------------------------------------------
// mmuMSFDocument
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int mmuMSFDocument::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  // here should write elements specific for the document
  mafStorageElement *root_elem=node->StoreObject("Root",m_Root);
  return root_elem?MAF_OK:MAF_ERROR;
}

//------------------------------------------------------------------------------
int mmuMSFDocument::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------
{
  // here should restore elements specific for the document
  return node->RestoreObject("Root",m_Root);
}

//------------------------------------------------------------------------------
// mafMSFStorage
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafMSFStorage)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafMSFStorage::mafMSFStorage()
//------------------------------------------------------------------------------
{
  SetVersion("2.0");
  SetFileType("MSF");
  mafNEW(m_Root); // create a root node
  m_Root->SetName("Root");
  m_Root->SetListener(this);
  SetDocument(new mmuMSFDocument(m_Root)); // create a MSF doc and set the root node
}

//------------------------------------------------------------------------------
mafMSFStorage::~mafMSFStorage()
//------------------------------------------------------------------------------
{
  cppDEL(m_Document); // delete the document object
  mafDEL(m_Root);
}

//------------------------------------------------------------------------------
mafVMERoot *mafMSFStorage::GetRoot()
//------------------------------------------------------------------------------
{
  return m_Root;
}

//------------------------------------------------------------------------------
void mafMSFStorage::OnEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  // default forward events to 
  if (e->GetChannel()==MCH_UP)
  {
    // by default send events to listener
    InvokeEvent(e);
  }
}
