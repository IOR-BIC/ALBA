/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCoreFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-06 21:21:47 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafCoreFactory.h"
#include "mafVersion.h"
#include "mafIndent.h"
#include <string>
#include <ostream>
#include <sstream>

mafCoreFactory *mafCoreFactory::m_Instance=NULL;

mafCxxTypeMacro(mafCoreFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafCoreFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=mafCoreFactory::New();

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
mafCoreFactory::mafCoreFactory()
//------------------------------------------------------------------------------
{
  //
  // Plug here Objects in this factory
  //

  //mafPlugObject<object_type>("comment");
}

//------------------------------------------------------------------------------
const char* mafCoreFactory::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafCoreFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "MAF-Core Object Factory";
}
