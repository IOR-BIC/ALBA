/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-06 21:21:13 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafNodeFactory.h"
#include "mafVersion.h"
#include "mafNode.h"
#include "mafIndent.h"
#include <string>
#include <ostream>
#include <sstream>

mafNodeFactory *mafNodeFactory::m_Instance=NULL;

mafCxxTypeMacro(mafNodeFactory);

std::vector<std::string> mafNodeFactory::m_NodeNames;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafNodeFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=mafNodeFactory::New();

    if (m_Instance)
    {
      m_Instance->RegisterFactory(m_Instance);
      return MAF_OK;  
    }
    else
    {
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}

//------------------------------------------------------------------------
mafNodeFactory::mafNodeFactory()
//------------------------------------------------------------------------------
{
  //
  // Plug here Nodes in this factory
  //

  //mafPlugNodeMacro(node_type,"comment");
}

//------------------------------------------------------------------------------
const char* mafNodeFactory::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafNodeFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF Nodes";
}

//------------------------------------------------------------------------------
mafNode *mafNodeFactory::CreateInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafNode::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void mafNodeFactory::RegisterNewNode(const char* node_name, const char* description, mafCreateObjectFunction createFunction)
//------------------------------------------------------------------------------
{
  m_NodeNames.push_back(node_name);
  RegisterNewObject(node_name,description,createFunction);
}