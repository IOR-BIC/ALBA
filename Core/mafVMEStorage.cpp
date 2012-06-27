/*=========================================================================

 Program: MAF2
 Module: mafVMEStorage
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafIncludeWX.h" // to be removed

#include "mafVMEStorage.h"
#include "mafVMERoot.h"
#include "mafUtility.h"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mmuIdFactory.h"

MAF_ID_IMP(mafVMEStorage::MSF_FILENAME_CHANGED);

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
  m_Root->Shutdown(); // force shutdown to be then re-initialized...
  int ret = node->RestoreObject("Root",m_Root);
  if (ret==MAF_OK)
  {
    return m_Root->Initialize();
  }

  return MAF_ERROR; 
}

//------------------------------------------------------------------------------
// mafVMEStorage
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEStorage)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMEStorage::mafVMEStorage()
//------------------------------------------------------------------------------
{
  SetVersion("2.2");
  SetFileType("MSF");
  mafNEW(m_Root); // create a root node
  m_Root->SetName("Root");
  m_Root->SetListener(this);
  m_Root->Initialize();
  SetDocument(new mmuMSFDocument(m_Root)); // create a MSF doc and set the root node
}

//------------------------------------------------------------------------------
mafVMEStorage::~mafVMEStorage()
//------------------------------------------------------------------------------
{
  cppDEL(m_Document); // delete the document object
  mafDEL(m_Root); // delete the root
}

//------------------------------------------------------------------------------
void mafVMEStorage::SetRoot(mafVMERoot *root)
//------------------------------------------------------------------------------
{
  if (root == m_Root)
    return;
  
  cppDEL(m_Document); // delete the old msf document
  mafDEL(m_Root); // delete the old root
  // assign the new root
  m_Root = root;
  m_Root->Register(this);
  m_Root->SetName("Root");
  m_Root->SetListener(this);
  m_Root->Initialize();
  
  SetDocument(new mmuMSFDocument(m_Root)); // assign the new document
}
//------------------------------------------------------------------------------
mafVMERoot *mafVMEStorage::GetRoot()
//------------------------------------------------------------------------------
{
  return m_Root;
}

//------------------------------------------------------------------------------
void mafVMEStorage::SetURL(const char *name)
//------------------------------------------------------------------------------
{
  if (m_URL!=name)
  {
    Superclass::SetURL(name);
    // forward down the event informing that msf file name is changed
    mafEventBase e(this,MSF_FILENAME_CHANGED);
    GetRoot()->ForwardDownEvent(e);
  }
}

//------------------------------------------------------------------------------
void mafVMEStorage::OnEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  // default forward events to 
  if (e->GetChannel()==MCH_UP)
  {
    // by default send events to listener
    InvokeEvent(e);
  }
}
