/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeRoot.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:27:14 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafNodeRoot_cxx
#define __mafNodeRoot_cxx

#include "mafNodeRoot.h"
#include "mafIndent.h"
#include "mafAttribute.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafNodeRoot);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafNodeRoot::mafNodeRoot()
//-------------------------------------------------------------------------
{
  m_MaxItemId=-1;
  m_MaxNodeId=-1;
}

//-------------------------------------------------------------------------
mafNodeRoot::~mafNodeRoot()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
int mafNodeRoot::ReparentTo(mafNode *parent)
//-------------------------------------------------------------------------
{
/*  mafNode *node=mafNodeGroup::New();
  node->DeepCopy(this);
  //node->Import(this);

  return node->ReparentTo(parent);*/
  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
void mafNodeRoot::CleanTree()
//-------------------------------------------------------------------------
{
  Superclass::CleanTree();
  m_Attributes.clear();
}


//-------------------------------------------------------------------------
void mafNodeRoot::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  mafNode::Print(os,tabs);
  os << mafIndent(tabs) << "MaxItemId: " << m_MaxItemId << "\n";
  os << mafIndent(tabs) << "MaxNodeId: " << m_MaxNodeId << "\n";
}
  
#endif