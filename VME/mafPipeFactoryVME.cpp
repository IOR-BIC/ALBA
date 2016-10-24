/*=========================================================================

 Program: MAF2
 Module: mafPipeFactoryVME
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
#include "mafPipePolylineSlice.h"
#include "mafPipeSurfaceTextured.h"

#include "mafPipeVolumeProjected.h"
#include "mafPipeMeter.h"
#include "mafPipeGizmo.h"
#include "mafPipeLandmarkCloud.h"
#include "mafPipePointSet.h"
#include "mafPipePolyline.h"
#include "mafPipeIsosurface.h"
#ifdef WIN32
#include "mafPipeIsosurfaceGPU.h"
#endif
#include "mafPipeImage3D.h"
#include "mafPipeVector.h"
#include "mafPipeMesh.h"
#include "mafPipeMeshSlice.h"
#include "mafPipeScalar.h"
#include "mafVisualPipeVolumeRayCasting.h"

#ifdef MAF_USE_ITK
  #include "mafPipeScalarMatrix.h"
#endif

#include "mafPipeVolumeMIP.h"
#include "mafPipeVolumeDRR.h"
#include "mafPipeVolumeVR.h"
#include "mafPipeTrajectories.h"

#include "mafPipeWrappedMeter.h"
#include "mafPipeSurfaceEditor.h"
#include "mafVisualPipeSlicerSlice.h"
#include "mafVisualPipePolylineGraph.h"

#include "mafPipeDensityDistance.h"

#ifdef MAF_USE_ITK  
#include "mafPipeGraph.h"
#endif

#include "mafPipePolylineGraphEditor.h"

#include "mafPipeMeshSlice_BES.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeCompoundVolume.h"
#include "mafPipeCompoundVolumeFixedScalars.h"
#include "mafPipeVectorFieldGlyphs.h"
#include "mafPipeVectorFieldSurface.h"
#include "mafPipeVectorFieldMapWithArrows.h"
#include "mafPipeVectorFieldSlice.h"
#include "mafPipeTensorFieldGlyphs.h"
#include "mafPipeTensorFieldSurface.h"
#include "mafPipeTensorFieldSlice.h"
#include "mafPipeVolumeSliceBlend.h"
#include "mafVisualPipeCollisionDetection.h"
#include "mafPipeVolumeSliceNotInterpolated.h"

#include "mafPipeRayCast.h"

// local include
#include <string>
#include <ostream>
#include "mafPipeRefSys.h"

//mafPipeFactoryVME *mafPipeFactoryVME::m_Instance=NULL;

bool mafPipeFactoryVME::m_Initialized=false;

mafCxxTypeMacro(mafPipeFactoryVME);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafPipeFactoryVME::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    //m_Instance=mafPipeFactoryVME::New();

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
mafPipeFactoryVME::mafPipeFactoryVME()
//------------------------------------------------------------------------------
{
  //m_Instance = NULL;
  
  //
  // Plug here Pipes in this factory
  //
  mafPlugPipeMacro(mafPipeSurface,"Pipe to render vtk polydata as surface rendering");
	mafPlugPipeMacro(mafPipeRefSys,"Pipe for VME RefSys");
  mafPlugPipeMacro(mafPipeSurfaceSlice,"Pipe for render vtk surface as an arbitrary slice");
  mafPlugPipeMacro(mafPipeSurfaceTextured,"Pipe for render vtk polydata as textured surface rendering");
	mafPlugPipeMacro(mafPipePolylineSlice,"Pipe for render vtk polyline as an arbitrary slice");
  mafPlugPipeMacro(mafPipeGizmo,"Pipe to render gizmos");
  mafPlugPipeMacro(mafPipeMeter,"Pipe for mafVMEMeter");
  mafPlugPipeMacro(mafPipeVolumeProjected,"Pipe for render vtk volumes as a projected image along X,Y or Z axes");
  mafPlugPipeMacro(mafPipePointSet,"Pipe for render vtk pointset");
  mafPlugPipeMacro(mafPipePolyline,"Pipe for render polylines");
  mafPlugPipeMacro(mafPipeVector,"Pipe to render vectors ");
  mafPlugPipeMacro(mafPipeLandmarkCloud,"Pipe for render landmark clouds");
  mafPlugPipeMacro(mafPipeIsosurface,"Pipe for render vtk volumes as a iso-surface");
#ifdef WIN32
  mafPlugPipeMacro(mafPipeIsosurfaceGPU,"Pipe for render vtk volumes as a iso-surface through a GPU");
#endif
  mafPlugPipeMacro(mafPipeImage3D,"Pipe for render images");
	mafPlugPipeMacro(mafPipeMesh,"Pipe for render Mesh");
	mafPlugPipeMacro(mafPipeMeshSlice,"Pipe for render a slice of a Mesh");
  mafPlugPipeMacro(mafPipeScalar,"Pipe for render scalar data");
  mafPlugPipeMacro(mafVisualPipeVolumeRayCasting,"Pipe for render volume data with Ray Cast method.");

#ifdef MAF_USE_ITK
  mafPlugPipeMacro(mafPipeScalarMatrix,"Pipe for render matrix scalar data");
#endif

  mafPlugPipeMacro(mafPipeVolumeMIP, "Pipe for render vtk volumes with MIP ray cast method.");
  mafPlugPipeMacro(mafPipeVolumeDRR, "Pipe for render vtk volumes with XRay cast method.");

  mafPlugPipeMacro(mafPipeVolumeVR, "Pipe for render vtk volumes with Volume Rendere cast method.");

  mafPlugPipeMacro(mafPipeDensityDistance, "Pipe for visualize the value of scalars on a surface.");

  mafPlugPipeMacro(mafPipeTrajectories, "Pipe to render animated Landmark trajectories in a time interval.");
  mafPlugPipeMacro(mafPipePolylineGraphEditor, "Pipe to Visualize Polyline/Graph in way to edit them.");

  mafPlugPipeMacro(mafPipeWrappedMeter, "Pipe to Visualize Wrapped Meter");
  mafPlugPipeMacro(mafPipeSurfaceEditor, "Pipe to Visualize Surface in way to edit them.");

  mafPlugPipeMacro(mafVisualPipeSlicerSlice, "Pipe to Visualize Slicer as borders cutted from a plane.");

  mafPlugPipeMacro(mafVisualPipePolylineGraph, "Pipe to Visualize Polyline and Graph.");


#ifdef MAF_USE_ITK  

  mafPlugPipeMacro(mafPipeGraph, "Pipe to plot scalar graphics.");

#endif

  mafPlugPipeMacro(mafPipeCompoundVolume, "Compound pipe for rendering volumes.");
  mafPlugPipeMacro(mafPipeCompoundVolumeIsosurface,"Compound Pipe for render vtk volumes as a iso-surface");
  mafPlugPipeMacro(mafPipeCompoundVolumeMIP, "Compound pipe for render vtk volumes with MIP ray cast method.");
  mafPlugPipeMacro(mafPipeCompoundVolumeDRR, "Compound pipe for render vtk volumes with XRay cast method.");
  mafPlugPipeMacro(mafPipeCompoundVolumeVR, "Compound pipe for render vtk volumes with Volume Rendere cast method.");
  mafPlugPipeMacro(mafPipeVectorFieldGlyphs, "Pipe for rendering of vector fields using various glyphs.");
  mafPlugPipeMacro(mafPipeVectorFieldSurface, "Pipe for rendering of vector fields using color mapping on the object surface.");
  mafPlugPipeMacro(mafPipeVectorFieldMapWithArrows, "Pipe for rendering of vector and scalar fields using colored arrows mapping on the object surface.");
  mafPlugPipeMacro(mafPipeVectorFieldSlice, "Pipe for rendering of vector fields using color mapping on the slice of the object.");
  mafPlugPipeMacro(mafPipeTensorFieldGlyphs, "Pipe for rendering of tensor fields using glyphs.");
  mafPlugPipeMacro(mafPipeTensorFieldSurface, "Pipe for rendering of tensor fields using color mapping on the object surface.");
  mafPlugPipeMacro(mafPipeTensorFieldSlice, "Pipe for rendering of tensor fields using color mapping on the slice of the object.");
  mafPlugPipeMacro(mafPipeVolumeSliceBlend, "Pipe for rendering volume with 2 slices with opacity.");
  mafPlugPipeMacro(mafVisualPipeCollisionDetection, "Pipe for visualization of collision between 2 surfaces.");


  //BES: 16.4.2008 - these pipes are to be committed down (without _BES suffix) to openMAF in the future
  mafPlugPipeMacro(mafPipeVolumeSlice, "BES: mafPipeVolumeSlice.");
  mafPlugPipeMacro(mafPipeSurfaceSlice_BES, "BES: mafPipeSurfaceSlice.");
  mafPlugPipeMacro(mafPipePolylineSlice_BES, "BES: mafPipePolylineSlice_BES.");
  mafPlugPipeMacro(mafPipeMeshSlice_BES, "BES: mafPipeMeshSlice_BES.");

  mafPlugPipeMacro(mafPipeVolumeSliceNotInterpolated, "Pipe for not interpolated and not resampled volume visualization");   

  mafPlugPipeMacro(mafPipeRayCast, "Pipe for RayCast Volume rendering of bone-blood-muscle");
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
//------------------------------------------------------------------------------
mafPipeFactoryVME* mafPipeFactoryVME::GetInstance()
//------------------------------------------------------------------------------
{
  static mafPipeFactoryVME &istance = *(mafPipeFactoryVME::New());
  Initialize();
  return &istance;
}
