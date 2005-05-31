/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-31 23:47:20 $
  Version:   $Revision: 1.5 $
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


#include "mafNodeFactory.h"
#include "mafVersion.h"
#include "mafNodeRoot.h"
#include "mafNodeGeneric.h"
#include "mafTagArray.h"
#include "mafIndent.h"
#include <string>
#include <ostream>

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
  m_Instance = NULL;
  
  //
  // Plug here Nodes in this factory
  //
  //mafPlugNodeMacro(mafNodeRoot,"root for generic nodes tree");
  //mafPlugNodeMacro(mafNodeGeneric,"a generic node with only basic features");
  //mafPlugObjectMacro(mafTagArray,"a basic kind of attribute used to store key-value pairs");
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
mafNode *mafNodeFactory::CreateNodeInstance(const char *type_name)
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
