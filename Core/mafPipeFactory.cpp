/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2008-05-13 12:00:37 $
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


#include "mafPipeFactory.h"
#include "mafVersion.h"
#include "mafIndent.h"
#include "mafPipe.h"
#ifdef MAF_USE_VTK
  #include "mafPipeBox.h"
#endif
#include <string>
#include <ostream>
#include <algorithm>

mafPipeFactory *mafPipeFactory::m_Instance=NULL;

mafCxxTypeMacro(mafPipeFactory);

std::vector<std::string> mafPipeFactory::m_PipeNames;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafPipeFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=mafPipeFactory::New();

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
mafPipeFactory::mafPipeFactory()
//------------------------------------------------------------------------------
{
  m_Instance = NULL;
  
  //
  // Plug here Pipes in this factory
  //
  //mafPlugPipeMacro(mafPipeRoot,"root for generic pipes tree");
  //mafPlugPipeMacro(mafPipeGeneric,"a generic pipe with only basic features");
#ifdef MAF_USE_VTK
  mafPlugPipeMacro(mafPipeBox,"General pipe to show box for vtk data");
#endif
}

//------------------------------------------------------------------------------
const char* mafPipeFactory::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafPipeFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF Pipes";
}

//------------------------------------------------------------------------------
mafPipe *mafPipeFactory::CreatePipeInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafPipe::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void mafPipeFactory::RegisterNewPipe(const char* pipe_name, const char* description, mafCreateObjectFunction createFunction)
//------------------------------------------------------------------------------
{
	std::vector<std::string, std::allocator<std::string> >::const_iterator it = std::find(m_PipeNames.begin (), m_PipeNames.end (), std::string(pipe_name));
	if(it._Myptr != 0 && it != m_PipeNames.end() )
	{
    return;
	}
  m_PipeNames.push_back(pipe_name);
  RegisterNewObject(pipe_name,description,createFunction);
}
