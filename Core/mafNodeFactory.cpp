/*=========================================================================

 Program: MAF2
 Module: mafNodeFactory
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


#include "mafNodeFactory.h"
#include "mafVersion.h"
#include "mafNodeRoot.h"
#include "mafNodeLayout.h"
#include "mafNodeGeneric.h"
#include "mafTagArray.h"
#include "mafIndent.h"
#include <string>
#include <ostream>
#include <algorithm>

bool mafNodeFactory::m_Initialized=false;
// mafNodeFactory *mafNodeFactory::m_Instance=NULL;

mafCxxTypeMacro(mafNodeFactory);

// std::vector<std::string> mafNodeFactory::m_NodeNames;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafNodeFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    // m_Instance=mafNodeFactory::New();

    m_Initialized=true;
    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      return MAF_OK;  
    }
    else
    {
      m_Initialized=false;
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}

//------------------------------------------------------------------------
mafNodeFactory::mafNodeFactory()
//------------------------------------------------------------------------------
{
  // m_Instance = NULL;
  
  //
  // Plug here Nodes in this factory
  //
  //mafPlugNodeMacro(mafNodeRoot,"root for generic nodes tree");
  //mafPlugNodeMacro(mafNodeGeneric,"a generic node with only basic features");
  //mafPlugObjectMacro(mafTagArray,"a basic kind of attribute used to store key-value pairs");
  mafPlugNodeMacro(mafNodeLayout,"node for layout list");
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
  std::vector<std::string, std::allocator<std::string> >::const_iterator it = std::find(GetNodeNames().begin (), GetNodeNames().end (), std::string(node_name));
  if(it != GetNodeNames().end() )
  {
    return;
  }
  GetNodeNames().push_back(node_name);
  RegisterNewObject(node_name,description,createFunction);
}
//------------------------------------------------------------------------------
mafNodeFactory* mafNodeFactory::GetInstance()
//------------------------------------------------------------------------------
{
  static mafNodeFactory &istance = *(mafNodeFactory::New());
  Initialize();
  return &istance;
}
//------------------------------------------------------------------------------
std::vector<std::string> & mafNodeFactory::GetNodeNames()
//------------------------------------------------------------------------------
{
  static std::vector<std::string> nodeNames;
  return nodeNames;
}
