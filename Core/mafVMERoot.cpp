/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERoot.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:25:54 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMERoot_cxx
#define __mafVMERoot_cxx

#include "mafVMERoot.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMERoot)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMERoot::mafVMERoot()
//-------------------------------------------------------------------------
{
   m_MaxItemId=-1;
}

//-------------------------------------------------------------------------
mafVMERoot::~mafVMERoot()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void mafVMERoot::Print(std::ostream& os, const int tabs)
//-------------------------------------------------------------------------
{
  mafNode::Print(os,tabs);
  os << mafIndent(tabs) << "MaxItemId: " << m_MaxItemId << "\n";
  os << mafIndent(tabs) << "MaxNodeId: " << m_MaxNodeId << "\n";
}


#endif