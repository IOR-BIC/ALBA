/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeFactoryVME.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-13 11:07:32 $
  Version:   $Revision: 1.13 $
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


#include "mafPipeFactoryVME.h"
#include "mafVersion.h"
#include "mafIndent.h"

#include "mafPipe.h"
#include "mafPipeSurface.h"
#include "mafPipeSurfaceSlice.h"

#include "mafPipeVolumeSlice.h"
#include "mafPipeVolumeProjected.h"
#include "mafPipeMeter.h"
#include "mafPipeGizmo.h"
#include "mafPipeLandmarkCloud.h"
#include "mafPipePointSet.h"
#include "mafPipePolyline.h"
#include "mafPipeIsosurface.h"
#include "mafPipeImage3D.h"

#include <string>
#include <ostream>

mafPipeFactoryVME *mafPipeFactoryVME::m_Instance=NULL;

mafCxxTypeMacro(mafPipeFactoryVME);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafPipeFactoryVME::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=mafPipeFactoryVME::New();

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
mafPipeFactoryVME::mafPipeFactoryVME()
//------------------------------------------------------------------------------
{
  m_Instance = NULL;
  
  //
  // Plug here Pipes in this factory
  //
  mafPlugPipeMacro(mafPipeSurface,"Pipe to render vtk polydata as surface rendering");
  mafPlugPipeMacro(mafPipeSurfaceSlice,"Pipe for render vtk surface as an arbitrary slice");
  mafPlugPipeMacro(mafPipeGizmo,"Pipe to render gizmos");
  mafPlugPipeMacro(mafPipeMeter,"Pipe for mafVMEMeter");
  mafPlugPipeMacro(mafPipeVolumeSlice,"Pipe for render vtk volumes as an arbitrary slice");
  mafPlugPipeMacro(mafPipeVolumeProjected,"Pipe for render vtk volumes as a projected image along X,Y or Z axes");
  mafPlugPipeMacro(mafPipePointSet,"Pipe for render vtk pointset");
  mafPlugPipeMacro(mafPipePolyline,"Pipe for render polylines");
  mafPlugPipeMacro(mafPipeLandmarkCloud,"Pipe for render landmark clouds");
  mafPlugPipeMacro(mafPipeIsosurface,"Pipe for render vtk volumes as a iso-surface");
  mafPlugPipeMacro(mafPipeImage3D,"Pipe for render images");
}

//------------------------------------------------------------------------------
const char* mafPipeFactoryVME::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafPipeFactoryVME::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF Pipes of VME library";
}
