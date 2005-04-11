/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 16:43:20 $
  Version:   $Revision: 1.1 $
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

#include "mafVMEFactory.h"
#include "mafVersion.h"
#include "mafVMERoot.h"
#include "mafVMEGenericVTK.h"
#include "mafTagArray.h"
#include "mafIndent.h"
#include <string>
#include <ostream>
#include <sstream>

mafCxxTypeMacro(mafVMEFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafVMEFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=mafVMEFactory::New();

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
mafVMEFactory::mafVMEFactory()
//------------------------------------------------------------------------------
{
  //
  // Plug here Nodes in this factory
  //
  mafPlugObjectMacro(mafTagArray,"a basic kind of attribute used to store key-value pairs");
  mafPlugNodeMacro(mafVMERoot,"root for VME tree");
  mafPlugNodeMacro(mafVMEGenericVTK,"Generic VME storing VTK datasets");
}

//------------------------------------------------------------------------------
const char* mafVMEFactory::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafVMEFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF VMEs";
}

//------------------------------------------------------------------------------
mafVME *mafVMEFactory::CreateVMEInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(Superclass::CreateInstance(type_name));
}
