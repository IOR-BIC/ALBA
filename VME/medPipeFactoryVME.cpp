/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeFactoryVME.cpp,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:27:37 $
  Version:   $Revision: 1.13.2.8 $
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
#include "medPipeVolumeVR.h"
#include "medPipeTrajectories.h"

#include "medPipeWrappedMeter.h"
#include "medPipeSurfaceEditor.h"
#include "medVisualPipeSlicerSlice.h"
#include "medVisualPipePolylineGraph.h"



#include "medPipeDensityDistance.h"

#ifdef MAF_USE_ITK  
  #include "medPipeGraph.h"
#endif

#include "medPipePolylineGraphEditor.h"

//TODO: commit down to openMAF
#include "mafPipeMeshSlice_BES.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafPipeVolumeSlice_BES.h"
///////

#include "medPipeCompoundVolume.h"
#include "medPipeCompoundVolumeFixedScalars.h"
#include "medPipeVectorFieldGlyphs.h"
#include "medPipeVectorFieldSurface.h"
#include "medPipeVectorFieldMapWithArrows.h"
#include "medPipeVectorFieldSlice.h"
#include "medPipeTensorFieldGlyphs.h"
#include "medPipeTensorFieldSurface.h"
#include "medPipeTensorFieldSlice.h"

#include <string>
#include <ostream>

mafCxxTypeMacro(medPipeFactoryVME);

bool medPipeFactoryVME::m_Initialized=false;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int medPipeFactoryVME::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    m_Initialized=true;
    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      return MAF_OK;  
    }
    else
    {
      m_Initialized=false;
      return MAF_ERROR;
    }
  }

  return MAF_OK;
}

//------------------------------------------------------------------------
medPipeFactoryVME::medPipeFactoryVME()
//------------------------------------------------------------------------------
{ 
  //
  // Plug here Pipes in this factory
  //
  mafPlugPipeMacro(medPipeVolumeMIP, "Pipe for render vtk volumes with MIP ray cast method.");
  mafPlugPipeMacro(medPipeVolumeDRR, "Pipe for render vtk volumes with XRay cast method.");

	mafPlugPipeMacro(medPipeVolumeVR, "Pipe for render vtk volumes with Volume Rendere cast method.");

	mafPlugPipeMacro(medPipeDensityDistance, "Pipe for visualize the value of scalars on a surface.");

  mafPlugPipeMacro(medPipeTrajectories, "Pipe to render animated Landmark trajectories in a time interval.");
	mafPlugPipeMacro(medPipePolylineGraphEditor, "Pipe to Visualize Polyline/Graph in way to edit them.");

	mafPlugPipeMacro(medPipeWrappedMeter, "Pipe to Visualize Wrapped Meter");
	mafPlugPipeMacro(medPipeSurfaceEditor, "Pipe to Visualize Surface in way to edit them.");

  mafPlugPipeMacro(medVisualPipeSlicerSlice, "Pipe to Visualize Slicer as borders cutted from a plane.");

  mafPlugPipeMacro(medVisualPipePolylineGraph, "Pipe to Visualize Polyline and Graph.");


#ifdef MAF_USE_ITK  

  mafPlugPipeMacro(medPipeGraph, "Pipe to plot scalar graphics.");

#endif

  mafPlugPipeMacro(medPipeCompoundVolume, "Compound pipe for rendering volumes.");
  mafPlugPipeMacro(medPipeCompoundVolumeIsosurface,"Compound Pipe for render vtk volumes as a iso-surface");
  mafPlugPipeMacro(medPipeCompoundVolumeMIP, "Compound pipe for render vtk volumes with MIP ray cast method.");
  mafPlugPipeMacro(medPipeCompoundVolumeDRR, "Compound pipe for render vtk volumes with XRay cast method.");
  mafPlugPipeMacro(medPipeCompoundVolumeVR, "Compound pipe for render vtk volumes with Volume Rendere cast method.");
  mafPlugPipeMacro(medPipeVectorFieldGlyphs, "Pipe for rendering of vector fields using various glyphs.");
  mafPlugPipeMacro(medPipeVectorFieldSurface, "Pipe for rendering of vector fields using color mapping on the object surface.");
  mafPlugPipeMacro(medPipeVectorFieldMapWithArrows, "Pipe for rendering of vector and scalar fields using colored arrows mapping on the object surface.");
  mafPlugPipeMacro(medPipeVectorFieldSlice, "Pipe for rendering of vector fields using color mapping on the slice of the object.");
  mafPlugPipeMacro(medPipeTensorFieldGlyphs, "Pipe for rendering of tensor fields using glyphs.");
  mafPlugPipeMacro(medPipeTensorFieldSurface, "Pipe for rendering of tensor fields using color mapping on the object surface.");
  mafPlugPipeMacro(medPipeTensorFieldSlice, "Pipe for rendering of tensor fields using color mapping on the slice of the object.");


  //BES: 16.4.2008 - these pipes are to be committed down (without _BES suffix) to openMAF in the future
  mafPlugPipeMacro(mafPipeVolumeSlice_BES, "BES: mafPipeVolumeSlice_BES.");
  mafPlugPipeMacro(mafPipeSurfaceSlice_BES, "BES: mafPipeSurfaceSlice.");
  mafPlugPipeMacro(mafPipePolylineSlice_BES, "BES: mafPipePolylineSlice_BES.");
  mafPlugPipeMacro(mafPipeMeshSlice_BES, "BES: mafPipeMeshSlice_BES.");   
}
/*
//------------------------------------------------------------------------------
const char* medPipeFactoryVME::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}
*/
//------------------------------------------------------------------------------
const char* medPipeFactoryVME::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for medical Pipes of medVME library";
}
//------------------------------------------------------------------------------
medPipeFactoryVME* medPipeFactoryVME::GetInstance()
//------------------------------------------------------------------------------
{
  static medPipeFactoryVME &istance = *(medPipeFactoryVME::New());
  Initialize();
  return &istance;
}