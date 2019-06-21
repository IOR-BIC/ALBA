/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEStorage
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaIncludeWX.h" // to be removed

#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaUtility.h"
#include "albaStorable.h"
#include "albaStorageElement.h"
#include "mmuIdFactory.h"

ALBA_ID_IMP(albaVMEStorage::MSF_FILENAME_CHANGED);

//------------------------------------------------------------------------------
// mmuMSFDocument
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int mmuMSFDocument::InternalStore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  // here should write elements specific for the document
  albaStorageElement *root_elem=node->StoreObject("Root",m_Root);
  return root_elem?ALBA_OK:ALBA_ERROR;
}

//------------------------------------------------------------------------------
int mmuMSFDocument::InternalRestore(albaStorageElement *node)
//-------------------------------------------------------
{
  // here should restore elements specific for the document
  m_Root->Shutdown(); // force shutdown to be then re-initialized...
  int ret = node->RestoreObject("Root",m_Root);
  if (ret==ALBA_OK)
  {
    return m_Root->Initialize();
  }

  return ALBA_ERROR; 
}

//------------------------------------------------------------------------------
// albaVMEStorage
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEStorage)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaVMEStorage::albaVMEStorage()
//------------------------------------------------------------------------------
{
  SetVersion("2.2");
  SetFileType("MSF");
  albaNEW(m_Root); // create a root node
  m_Root->SetName("Root");
  m_Root->SetStorage(this);
  m_Root->Initialize();
  SetDocument(new mmuMSFDocument(m_Root)); // create a MSF doc and set the root node
}

//------------------------------------------------------------------------------
albaVMEStorage::~albaVMEStorage()
//------------------------------------------------------------------------------
{
	m_Root->SetStorage(NULL);

  cppDEL(m_Document); // delete the document object
	albaDEL(m_Root); // delete the root
}

//------------------------------------------------------------------------------
void albaVMEStorage::SetRoot(albaVMERoot *root)
//------------------------------------------------------------------------------
{
  if (root == m_Root)
    return;
  
  cppDEL(m_Document); // delete the old msf document
  albaDEL(m_Root); // delete the old root
  // assign the new root
  m_Root = root;
  m_Root->Register(this);
  m_Root->SetName("Root");
  m_Root->SetStorage(this);
  m_Root->Initialize();
  
  SetDocument(new mmuMSFDocument(m_Root)); // assign the new document
}
//------------------------------------------------------------------------------
albaVMERoot *albaVMEStorage::GetRoot()
//------------------------------------------------------------------------------
{
  return m_Root;
}

//------------------------------------------------------------------------------
void albaVMEStorage::SetURL(const char *name)
//------------------------------------------------------------------------------
{
  if (m_URL!=name)
  {
    Superclass::SetURL(name);
    // forward down the event informing that msf file name is changed
    albaEventBase e(this,MSF_FILENAME_CHANGED);
    GetRoot()->ForwardDownEvent(e);
  }
}

//------------------------------------------------------------------------------
void albaVMEStorage::OnEvent(albaEventBase *e)
//------------------------------------------------------------------------------
{
  // default forward events to 
  if (e->GetChannel()==MCH_UP)
  {
    // by default send events to listener
    InvokeEvent(e);
  }
}
