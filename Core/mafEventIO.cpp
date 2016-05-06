/*=========================================================================

 Program: MAF2
 Module: mafEventIO
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafEventIO.h"
#include "mafVME.h"
#include "mafRoot.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafEventIO)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafEventIO::mafEventIO(void *sender, mafID id, mafID item_id, void *data, mafID channel):
  mafEventBase(sender,id,data,channel)
//------------------------------------------------------------------------------
{
  m_ItemId = item_id;
  m_Storage = NULL;
  m_Root    = NULL;
}

//------------------------------------------------------------------------------
mafEventIO::~mafEventIO()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafEventIO::DeepCopy(const mafEventIO *c)
//------------------------------------------------------------------------------
{
  Superclass::DeepCopy(c);
  m_ItemId = c->m_ItemId;
}

//------------------------------------------------------------------------------
void mafEventIO::SetItemId(mafID id)
//------------------------------------------------------------------------------
{
  m_ItemId = id;
}
//------------------------------------------------------------------------------
mafID mafEventIO::GetItemId()
//------------------------------------------------------------------------------
{
  return m_ItemId;
}

//------------------------------------------------------------------------------
void mafEventIO::SetStorage(mafStorage *storage)
//------------------------------------------------------------------------------
{
  m_Storage = storage;
}
//------------------------------------------------------------------------------
mafStorage *mafEventIO::GetStorage()
//------------------------------------------------------------------------------
{
  return m_Storage;
}

//------------------------------------------------------------------------------
void mafEventIO::SetRoot(mafVME *root)
//------------------------------------------------------------------------------
{
  try 
  { 
    dynamic_cast<mafRoot *>(root); // test if it's a root
    m_Root=root;
  } 
  catch (std::bad_cast) 
  { 
    m_Root = NULL;
  }
  
}
//------------------------------------------------------------------------------
mafVME *mafEventIO::GetRoot()
//------------------------------------------------------------------------------
{
  return m_Root;  
}
