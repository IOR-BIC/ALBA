/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCoreFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:22:14 $
  Version:   $Revision: 1.3 $
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

mafCoreFactory *mafCoreFactory::Instance=NULL;

mafCxxTypeMacro(mafCoreFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafCoreFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (Instance==NULL)
  {
    Instance=mafCoreFactory::New();

    if (Instance)
    {
      Instance->RegisterFactory(Instance);
      return MAF_OK;  
    }
    else
    {
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}

//----------------------------------------------------------------------------
void mafCoreFactory::PrintSelf(std::ostream& os, const int indent)
//------------------------------------------------------------------------------
{
  Superclass::Print(os,indent);
  os << mafIndent(indent) << "MAF Core object factory" << std::endl;
}


//------------------------------------------------------------------------
mafCoreFactory::mafCoreFactory()
//------------------------------------------------------------------------------
{
  //RegisterNewObject(objectName,"ObjectDescrition");
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
  return "MAF Core Object Factory";
}


//------------------------------------------------------------------------------
const char *mafObjectFactory::GetMAFSourceVersion(void) const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char *mafObjectFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "MAF core factory.";
}
