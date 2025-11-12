/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCoreFactory
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


#include "albaCoreFactory.h"
#include "albaIndent.h"
#include <string>
#include <ostream>

bool albaCoreFactory::m_Initialized=false;

albaCxxTypeMacro(albaCoreFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int albaCoreFactory::Initialize()
{
  if (m_Initialized)
  {
    // m_Instance=albaCoreFactory::New();

    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      m_Initialized=true;
      return ALBA_OK;  
    }
    else
    {
      return ALBA_ERROR;
    }
  }
  
  return ALBA_OK;
}

//------------------------------------------------------------------------
albaCoreFactory::albaCoreFactory()
{
  // m_Instance = NULL;
  
  //
  // Plug here Objects in this factory
  //
  //albaPlugObject<object_type>("comment");
}

//------------------------------------------------------------------------------
const char* albaCoreFactory::GetDescription() const
{
  return "ALBA-Core Object Factory";
}
//------------------------------------------------------------------------------
albaCoreFactory* albaCoreFactory::GetInstance()
{
  static albaCoreFactory &instance = *(albaCoreFactory::New());
  return &instance;
}
