/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeRoot.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-11 15:43:56 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafNodeRoot_cxx
#define __mafNodeRoot_cxx

#include "mafNodeRoot.h"
#include "mafNode.h"
#include "mafStorageElement.h"
#include "mafIndent.h"

//-------------------------------------------------------------------------
mafNodeRoot::mafNodeRoot()
//-------------------------------------------------------------------------
{
  m_MaxNodeId=0;
}

//-------------------------------------------------------------------------
mafNodeRoot::~mafNodeRoot()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mafNodeRoot* mafNodeRoot::SafeDownCast(mafObject *o)
//-------------------------------------------------------------------------
{
  try 
  { 
    return dynamic_cast<mafNodeRoot *>(o);
  } 
  catch (std::bad_cast) 
  { 
    return NULL;
  }
}

//-------------------------------------------------------------------------
int mafNodeRoot::StoreRoot(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute("MaxNodeId",mafString(m_MaxNodeId));
  return MAF_OK;
}
//-------------------------------------------------------------------------
int mafNodeRoot::RestoreRoot(mafStorageElement *element)
//-------------------------------------------------------------------------
{
  mafString max_id;
  if (!element->GetAttribute("MaxNodeId",max_id))
    return MAF_ERROR;

  SetMaxNodeId((mafID)atof(max_id));

  return MAF_OK;
}

//-------------------------------------------------------------------------
void mafNodeRoot::Print(std::ostream& os, const int tabs) const 
//-------------------------------------------------------------------------
{
  os << mafIndent(tabs) << "MaxNodeId: " << m_MaxNodeId << "\n";
}
  
#endif