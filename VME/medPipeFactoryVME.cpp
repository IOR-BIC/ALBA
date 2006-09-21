/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeFactoryVME.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-21 07:55:34 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
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


#include "medPipeFactoryVME.h"
#include "mafVersion.h"
#include "mafIndent.h"

#include "mafPipe.h"
#include "medPipeVolumeMIP.h"
#include "medPipeVolumeDRR.h"

#include "medPipeDensityDistance.h"

#include <string>
#include <ostream>

medPipeFactoryVME *medPipeFactoryVME::m_Instance=NULL;

mafCxxTypeMacro(medPipeFactoryVME);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int medPipeFactoryVME::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance == NULL)
  {
    m_Instance = medPipeFactoryVME::New();

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
medPipeFactoryVME::medPipeFactoryVME()
//------------------------------------------------------------------------------
{
  m_Instance = NULL;
  
  //
  // Plug here Pipes in this factory
  //
  mafPlugPipeMacro(medPipeVolumeMIP, "Pipe for render vtk volumes with MIP ray cast method.");
  mafPlugPipeMacro(medPipeVolumeDRR, "Pipe for render vtk volumes with XRay cast method.");

	mafPlugPipeMacro(medPipeDensityDistance, "Pipe for visualize the value of scalars on a surface.");
}

//------------------------------------------------------------------------------
const char* medPipeFactoryVME::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* medPipeFactoryVME::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for medical Pipes of medVME library";
}
