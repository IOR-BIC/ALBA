/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMSFStorage.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-14 18:16:05 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafIncludeWX.h" // to be removed

#include "mafMSFStorage.h"
#include "mafVMERoot.h"

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
  mafNEW(m_VMERoot);
  SetRoot(m_VMERoot);
  m_VMERoot->SetName("Root");
  m_VMERoot->SetListener(this);
}

//------------------------------------------------------------------------------
mafMSFStorage::~mafMSFStorage()
//------------------------------------------------------------------------------
{
  mafDEL(m_VMERoot);
}


//------------------------------------------------------------------------------
void mafMSFStorage::SetRoot(mafStorable *root)
//------------------------------------------------------------------------------
{
  Superclass::SetRoot(root);
}

//------------------------------------------------------------------------------
mafVMERoot *mafMSFStorage::GetRoot()
//------------------------------------------------------------------------------
{
  assert(m_VMERoot==m_Root);
  return m_VMERoot;
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
