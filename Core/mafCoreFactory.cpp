/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCoreFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2011-05-25 09:34:21 $
  Version:   $Revision: 1.7.24.1 $
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


#include "mafCoreFactory.h"
#include "mafVersion.h"
#include "mafIndent.h"
#include <string>
#include <ostream>

bool mafCoreFactory::m_Initialized=false;
// mafCoreFactory *mafCoreFactory::m_Instance=NULL;

mafCxxTypeMacro(mafCoreFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafCoreFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Initialized)
  {
    // m_Instance=mafCoreFactory::New();

    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      m_Initialized=true;
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
  // m_Instance = NULL;
  
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
//------------------------------------------------------------------------------
mafCoreFactory* mafCoreFactory::GetInstance()
//------------------------------------------------------------------------------
{
  static mafCoreFactory &instance = *(mafCoreFactory::New());
  return &instance;
}
