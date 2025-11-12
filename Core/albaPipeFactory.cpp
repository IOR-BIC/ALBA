/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeFactory
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


#include "albaPipeFactory.h"
#include "albaIndent.h"
#include "albaPipe.h"
#ifdef ALBA_USE_VTK
  #include "albaPipeBox.h"
#endif
#include <string>
#include <ostream>
#include <algorithm>

bool albaPipeFactory::m_Initialized=false;
// albaPipeFactory *albaPipeFactory::m_Instance=NULL;

albaCxxTypeMacro(albaPipeFactory);

// std::vector<std::string> albaPipeFactory::m_PipeNames;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int albaPipeFactory::Initialize()
{
  if (!m_Initialized)
  {
    // m_Instance=albaPipeFactory::New();

    m_Initialized=true;
    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      return ALBA_OK;  
    }
    else
    {
      m_Initialized=false;
      return ALBA_ERROR;
    }
  }
  
  return ALBA_OK;
}

//------------------------------------------------------------------------
albaPipeFactory::albaPipeFactory()
{
  // m_Instance = NULL;
  
  //
  // Plug here Pipes in this factory
  //
  //albaPlugPipeMacro(albaPipeRoot,"root for generic pipes tree");
  //albaPlugPipeMacro(albaPipeGeneric,"a generic pipe with only basic features");
#ifdef ALBA_USE_VTK
  albaPlugPipeMacro(albaPipeBox,"General pipe to show box for vtk data");
#endif
}

//------------------------------------------------------------------------------
const char* albaPipeFactory::GetDescription() const
{
  return "Factory for ALBA Pipes";
}

//------------------------------------------------------------------------------
albaPipe *albaPipeFactory::CreatePipeInstance(const char *type_name)
{
  return albaPipe::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void albaPipeFactory::RegisterNewPipe(const char* pipe_name, const char* description, albaCreateObjectFunction createFunction)
{
	std::vector<std::string, std::allocator<std::string> >::const_iterator it = std::find(GetPipeNames().begin (), GetPipeNames().end (), std::string(pipe_name));
	if(it != GetPipeNames().end() )
	{
    return;
	}
  GetPipeNames().push_back(pipe_name);
  RegisterNewObject(pipe_name,description,createFunction);
}
//------------------------------------------------------------------------------
albaPipeFactory* albaPipeFactory::GetInstance()
{
  static albaPipeFactory &istance = *(albaPipeFactory::New());
  Initialize();
  return &istance;
}
//------------------------------------------------------------------------------
std::vector<std::string> &albaPipeFactory::GetPipeNames()
{
  static std::vector<std::string> pipeNames;
  return pipeNames;
}
