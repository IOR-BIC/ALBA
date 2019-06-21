/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventIO
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "albaEventIO.h"
#include "albaVME.h"
#include "albaVMERoot.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaEventIO)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaEventIO::albaEventIO(void *sender, albaID id, albaID item_id, void *data, albaID channel):
  albaEventBase(sender,id,data,channel)
//------------------------------------------------------------------------------
{
  m_ItemId = item_id;
  m_Storage = NULL;
  m_Root    = NULL;
}

//------------------------------------------------------------------------------
albaEventIO::~albaEventIO()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaEventIO::DeepCopy(const albaEventIO *c)
//------------------------------------------------------------------------------
{
  Superclass::DeepCopy(c);
  m_ItemId = c->m_ItemId;
}

//------------------------------------------------------------------------------
void albaEventIO::SetItemId(albaID id)
//------------------------------------------------------------------------------
{
  m_ItemId = id;
}
//------------------------------------------------------------------------------
albaID albaEventIO::GetItemId()
//------------------------------------------------------------------------------
{
  return m_ItemId;
}

//------------------------------------------------------------------------------
void albaEventIO::SetStorage(albaStorage *storage)
//------------------------------------------------------------------------------
{
  m_Storage = storage;
}
//------------------------------------------------------------------------------
albaStorage *albaEventIO::GetStorage()
//------------------------------------------------------------------------------
{
  return m_Storage;
}

//------------------------------------------------------------------------------
void albaEventIO::SetRoot(albaVME *root)
//------------------------------------------------------------------------------
{
  try 
  { 
    dynamic_cast<albaVMERoot *>(root); // test if it's a root
    m_Root=root;
  } 
  catch (std::bad_cast) 
  { 
    m_Root = NULL;
  }
  
}
//------------------------------------------------------------------------------
albaVME *albaEventIO::GetRoot()
//------------------------------------------------------------------------------
{
  return m_Root;  
}
