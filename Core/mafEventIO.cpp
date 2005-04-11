/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventIO.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:15 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafEventIO.h"

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
}

//------------------------------------------------------------------------------
mafEventIO::~mafEventIO()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafEventIO::mafEventIO(const mafEventIO& c)
//------------------------------------------------------------------------------
{
  m_ItemId = c.m_ItemId;
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