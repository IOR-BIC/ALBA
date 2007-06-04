/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2007-06-04 15:11:54 $
  Version:   $Revision: 1.4 $
  Authors:   Daniele Giunchi
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

#include "medVMEFactory.h"

#include "mafVersion.h"

#include "mafVMEAdvancedProber.h"
#include "mafVMEAdvancedSlicer.h"

#ifdef MAF_USE_ITK
  #include "mafVMERawMotionData.h"
#endif


#include "mafIndent.h"
#include <string>
#include <ostream>

medVMEFactory *medVMEFactory::m_Instance=NULL;

mafCxxTypeMacro(medVMEFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int medVMEFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=medVMEFactory::New();

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
medVMEFactory::medVMEFactory()
//------------------------------------------------------------------------------
{
  //
  // Plug here Nodes in this factory
  //

#ifdef MAF_USE_ITK
  
#endif
  //mafPlugObjectMacro(mmaObject,"Object attributes");

  mafPlugNodeMacro(mafVMEAdvancedProber,"VME for Advanced Probing");
	mafPlugNodeMacro(mafVMEAdvancedSlicer,"VME for Advanced Slicing");
  

#ifdef MAF_USE_ITK
  mafPlugNodeMacro(mafVMERawMotionData,"VME that is a group for RawMotionData");
#endif
  //mafPlugNodeMacro(mafVMEthing,"VME representing a thing");
}

//------------------------------------------------------------------------------
const char* medVMEFactory::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* medVMEFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF VMEs";
}

//------------------------------------------------------------------------------
mafVME *medVMEFactory::CreateVMEInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(Superclass::CreateInstance(type_name));
}
