/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafRoot.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 12:59:56 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafRoot.h"
#include "mafNode.h"
#include "mafStorageElement.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafIndent.h"

//-------------------------------------------------------------------------
mafRoot::mafRoot()
//-------------------------------------------------------------------------
{
  m_MaxNodeId = 0;
}

//-------------------------------------------------------------------------
mafRoot::~mafRoot()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mafRoot* mafRoot::SafeDownCast(mafObject *o)
//-------------------------------------------------------------------------
{
  try 
  { 
    return dynamic_cast<mafRoot *>(o);
  } 
  catch (std::bad_cast) 
  { 
    return NULL;
  }
}

//-------------------------------------------------------------------------
int mafRoot::StoreRoot(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute("MaxNodeId",mafString(m_MaxNodeId));
  return MAF_OK;
}
//-------------------------------------------------------------------------
int mafRoot::RestoreRoot(mafStorageElement *element)
//-------------------------------------------------------------------------
{
  mafID max_id;
  if (!element->GetAttributeAsInteger("MaxNodeId",max_id))
    return MAF_ERROR;

  SetMaxNodeId(max_id);

  return MAF_OK;
}

//-------------------------------------------------------------------------
void mafRoot::OnRootEvent(mafEventBase *e)
//-------------------------------------------------------------------------
{
  if (e->GetChannel()==MCH_UP)
  {
    if (e->GetId()==NODE_GET_STORAGE)
    {
      // return the storage pointer: here the hypothesis sis the root node listener is a storage.
      mafEventIO *io_event=mafEventIO::SafeDownCast(e);
      io_event->SetStorage(GetStorage());
    }
    else
    {
      InvokeEvent(e);
    }
  }
}

//-------------------------------------------------------------------------
mafStorage *mafRoot::GetStorage()
//-------------------------------------------------------------------------
{
  return (typeid(m_Listener)==typeid(mafStorage *))?(mafStorage *)m_Listener:NULL;
}

//-------------------------------------------------------------------------
void mafRoot::Print(std::ostream& os, const int tabs) const 
//-------------------------------------------------------------------------
{
  os << mafIndent(tabs) << "MaxNodeId: " << m_MaxNodeId << "\n";
}
