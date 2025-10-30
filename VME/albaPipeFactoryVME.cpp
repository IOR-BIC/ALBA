/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeFactoryVME
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


#include "albaPipeFactoryVME.h"
#include "albaVersion.h"
#include "albaIndent.h"

#include "albaPipe.h"
#include "albaPipeSurface.h"
#include "albaPipeSurfaceTextured.h"

#include "albaPipeVolumeProjected.h"
#include "albaPipeMeter.h"
#include "albaPipeGizmo.h"
#include "albaPipeLandmarkCloud.h"
#include "albaPipePointSet.h"
#include "albaPipePointCloud.h"
#include "albaPipePolyline.h"
#include "albaPipeIsosurface.h"
#include "albaPipeImage3D.h"
#include "albaPipeVector.h"
#include "albaPipeMesh.h"
#include "albaPipeScalar.h"

#ifdef ALBA_USE_ITK
  #include "albaPipeScalarMatrix.h"
#endif

#include "albaPipeVolumeMIP.h"
#include "albaPipeVolumeDRR.h"
#include "albaPipeVolumeVR.h"
#include "albaPipeTrajectories.h"

#include "albaPipeWrappedMeter.h"
#include "albaPipeSurfaceEditor.h"
#include "albaVisualPipeSlicerSlice.h"
#include "albaVisualPipePolylineGraph.h"

#include "albaPipeDensityDistance.h"

#ifdef ALBA_USE_ITK  
#include "albaPipeGraph.h"
#endif

#include "albaPipePolylineGraphEditor.h"

#include "albaPipeMeshSlice.h"
#include "albaPipePolylineSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaPipeVolumeArbSlice.h"
#include "albaPipeVolumeArbOrthoSlice.h"
#include "albaPipeVolumeOrthoSlice.h"
#include "albaPipeCompoundVolume.h"
#include "albaPipeCompoundVolumeFixedScalars.h"
#include "albaPipeVectorFieldGlyphs.h"
#include "albaPipeVectorFieldSurface.h"
#include "albaPipeVectorFieldMapWithArrows.h"
#include "albaPipeVectorFieldSlice.h"
#include "albaPipeTensorFieldGlyphs.h"
#include "albaPipeTensorFieldSurface.h"
#include "albaPipeTensorFieldSlice.h"
#include "albaPipeVolumeSliceBlend.h"
#include "albaVisualPipeCollisionDetection.h"

#include "albaPipeRayCast.h"

// local include
#include <string>
#include <ostream>
#include "albaPipeRefSys.h"

//albaPipeFactoryVME *albaPipeFactoryVME::m_Instance=NULL;

bool albaPipeFactoryVME::m_Initialized=false;

albaCxxTypeMacro(albaPipeFactoryVME);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int albaPipeFactoryVME::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    //m_Instance=albaPipeFactoryVME::New();

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
albaPipeFactoryVME::albaPipeFactoryVME()
//------------------------------------------------------------------------------
{
  //m_Instance = NULL;
  
  //
  // Plug here Pipes in this factory
  //
  albaPlugPipeMacro(albaPipeSurface,"Pipe to render vtk polydata as surface rendering");
	albaPlugPipeMacro(albaPipeRefSys,"Pipe for VME RefSys");
  albaPlugPipeMacro(albaPipeSurfaceTextured,"Pipe for render vtk polydata as textured surface rendering");
  albaPlugPipeMacro(albaPipeGizmo,"Pipe to render gizmos");
  albaPlugPipeMacro(albaPipeMeter,"Pipe for albaVMEMeter");
  albaPlugPipeMacro(albaPipeVolumeProjected,"Pipe for render vtk volumes as a projected image along X,Y or Z axes");
  albaPlugPipeMacro(albaPipePointSet,"Pipe for render vtk pointset");
	albaPlugPipeMacro(albaPipePointCloud, "Pipe for render pointcloud");
  albaPlugPipeMacro(albaPipePolyline,"Pipe for render polylines");
  albaPlugPipeMacro(albaPipeVector,"Pipe to render vectors ");
  albaPlugPipeMacro(albaPipeLandmarkCloud,"Pipe for render landmark clouds");
  albaPlugPipeMacro(albaPipeIsosurface,"Pipe for render vtk volumes as a iso-surface");
  albaPlugPipeMacro(albaPipeImage3D,"Pipe for render images");
	albaPlugPipeMacro(albaPipeMesh,"Pipe for render Mesh");
  albaPlugPipeMacro(albaPipeScalar,"Pipe for render scalar data");

#ifdef ALBA_USE_ITK
  albaPlugPipeMacro(albaPipeScalarMatrix,"Pipe for render matrix scalar data");
#endif

  albaPlugPipeMacro(albaPipeVolumeMIP, "Pipe for render vtk volumes with MIP ray cast method.");
  albaPlugPipeMacro(albaPipeVolumeDRR, "Pipe for render vtk volumes with XRay cast method.");
  albaPlugPipeMacro(albaPipeVolumeVR, "Pipe for render vtk volumes with Volume Rendere cast method.");
  albaPlugPipeMacro(albaPipeDensityDistance, "Pipe for visualize the value of scalars on a surface.");
  albaPlugPipeMacro(albaPipeTrajectories, "Pipe to render animated Landmark trajectories in a time interval.");
  albaPlugPipeMacro(albaPipePolylineGraphEditor, "Pipe to Visualize Polyline/Graph in way to edit them.");
  albaPlugPipeMacro(albaPipeWrappedMeter, "Pipe to Visualize Wrapped Meter");
  albaPlugPipeMacro(albaPipeSurfaceEditor, "Pipe to Visualize Surface in way to edit them.");
  albaPlugPipeMacro(albaVisualPipeSlicerSlice, "Pipe to Visualize Slicer as borders cutted from a plane.");
  albaPlugPipeMacro(albaVisualPipePolylineGraph, "Pipe to Visualize Polyline and Graph.");


#ifdef ALBA_USE_ITK  
  albaPlugPipeMacro(albaPipeGraph, "Pipe to plot scalar graphics.");
#endif

  albaPlugPipeMacro(albaPipeCompoundVolume, "Compound pipe for rendering volumes.");
  albaPlugPipeMacro(albaPipeCompoundVolumeIsosurface,"Compound Pipe for render vtk volumes as a iso-surface");
  albaPlugPipeMacro(albaPipeCompoundVolumeMIP, "Compound pipe for render vtk volumes with MIP ray cast method.");
  albaPlugPipeMacro(albaPipeCompoundVolumeDRR, "Compound pipe for render vtk volumes with XRay cast method.");
  albaPlugPipeMacro(albaPipeCompoundVolumeVR, "Compound pipe for render vtk volumes with Volume Rendere cast method.");
  albaPlugPipeMacro(albaPipeVectorFieldGlyphs, "Pipe for rendering of vector fields using various glyphs.");
  albaPlugPipeMacro(albaPipeVectorFieldSurface, "Pipe for rendering of vector fields using color mapping on the object surface.");
  albaPlugPipeMacro(albaPipeVectorFieldMapWithArrows, "Pipe for rendering of vector and scalar fields using colored arrows mapping on the object surface.");
  albaPlugPipeMacro(albaPipeVectorFieldSlice, "Pipe for rendering of vector fields using color mapping on the slice of the object.");
  albaPlugPipeMacro(albaPipeTensorFieldGlyphs, "Pipe for rendering of tensor fields using glyphs.");
  albaPlugPipeMacro(albaPipeTensorFieldSurface, "Pipe for rendering of tensor fields using color mapping on the object surface.");
  albaPlugPipeMacro(albaPipeTensorFieldSlice, "Pipe for rendering of tensor fields using color mapping on the slice of the object.");
  albaPlugPipeMacro(albaPipeVolumeSliceBlend, "Pipe for rendering volume with 2 slices with opacity.");
  albaPlugPipeMacro(albaVisualPipeCollisionDetection, "Pipe for visualization of collision between 2 surfaces.");
	albaPlugPipeMacro(albaPipeVolumeArbSlice, "albaPipeVolumeArbSlice.");
	albaPlugPipeMacro(albaPipeVolumeArbOrthoSlice, "albaPipeVolumeArbOrthoSlice.");
	albaPlugPipeMacro(albaPipeVolumeOrthoSlice, "albaPipeVolumeOrthoSlice.");
	albaPlugPipeMacro(albaPipeSurfaceSlice, "albaPipeSurfaceSlice.");
  albaPlugPipeMacro(albaPipePolylineSlice, "albaPipePolylineSlice.");
  albaPlugPipeMacro(albaPipeMeshSlice, "albaPipeMeshSlice.");
  albaPlugPipeMacro(albaPipeRayCast, "Pipe for RayCast Volume rendering of bone-blood-muscle");
}

//------------------------------------------------------------------------------
const char* albaPipeFactoryVME::GetALBASourceVersion() const
//------------------------------------------------------------------------------
{
  return ALBA_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* albaPipeFactoryVME::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for ALBA Pipes of VME library";
}
//------------------------------------------------------------------------------
albaPipeFactoryVME* albaPipeFactoryVME::GetInstance()
//------------------------------------------------------------------------------
{
  static albaPipeFactoryVME &istance = *(albaPipeFactoryVME::New());
  Initialize();
  return &istance;
}
