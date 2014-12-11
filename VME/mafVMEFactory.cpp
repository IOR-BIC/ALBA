/*=========================================================================

 Program: MAF2
 Module: mafVMEFactory
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

#include "mafVMEFactory.h"

#include "mafVersion.h"
#include "mafNodeLayout.h"
#include "mafVMEExternalData.h"
#include "mafVMEGeneric.h"
#include "mafVMEGizmo.h"
#include "mafVMEGroup.h"
#include "mafVMEImage.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEMeter.h"
#include "mafVMEInfoText.h"
#include "mafVMEMesh.h"
#include "mafVMEPointSet.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMEProber.h"
#include "mafVMERefSys.h"
#include "mafVMERoot.h"

#ifdef MAF_USE_ITK
  #include "mafVMEScalarMatrix.h"
  #include "mafVMEItemScalarMatrix.h"
#endif

#include "mafVMEScalar.h"
#include "mafVMESlicer.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEVector.h"
#include "mafVMEVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"

#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mmaApplicationLayout.h"
#include "mafAttributeTraceability.h"
#include "mmaMeter.h"
#include "mafTagArray.h"
#include "mafVMEItemVTK.h"


#include "mafVMEWrappedMeter.h"
#include "mafVMEPolylineGraph.h"
#include "mafVMEPolylineEditor.h"
#include "mafVMEMaps.h"

#include "mafVMESurfaceEditor.h"
#include "mafVMELabeledVolume.h"
#include "mafVMESegmentationVolume.h"
#include "mafAttributeSegmentationVolume.h"

#ifdef MAF_USE_ITK
#include "mafVMERawMotionData.h"
#include "mafVMEAnalog.h"
#include "mafVMEStent.h"
#endif

#include "mafVMEVolumeLarge.h"

#include "mafIndent.h"
#include <string>
#include <ostream>


mafCxxTypeMacro(mafVMEFactory);

bool mafVMEFactory::m_Initialized=false;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafVMEFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    // m_Instance=mafVMEFactory::New();

    m_Initialized=true;
    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      return MAF_OK;  
    }
    else
    {
      m_Initialized=true;
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}

//------------------------------------------------------------------------
mafVMEFactory::mafVMEFactory()
//------------------------------------------------------------------------------
{
  //
  // Plug here Nodes in this factory
  //

  mafPlugNodeMacro(mafNodeLayout,"node for layout list");

  mafPlugObjectMacro(mafTagArray,"a basic kind of attribute used to store key-value pairs");
  mafPlugObjectMacro(mafVMEItemVTK,"a VME item storing VTK datasets");
#ifdef MAF_USE_ITK
  mafPlugObjectMacro(mafVMEItemScalarMatrix,"a VME item storing matrix scalar data through vnl matrix");
#endif
  mafPlugObjectMacro(mmaMeter,"Meter attributes");
  mafPlugObjectMacro(mmaMaterial,"Material attributes");
  mafPlugObjectMacro(mmaVolumeMaterial,"Volume material attributes");
  mafPlugObjectMacro(mmaApplicationLayout,"Applycation layout attributes");
  mafPlugObjectMacro(mafAttributeTraceability,"Trial event attributes");
  
  mafPlugNodeMacro(mafVMERoot,"root for VME tree");
  mafPlugNodeMacro(mafVMEExternalData,"VME used for referencing MAF external data.");
  mafPlugNodeMacro(mafVMEGroup,"VME used for composing assemblies");
  mafPlugNodeMacro(mafVMEGeneric,"Generic VME storing VTK datasets");
  mafPlugNodeMacro(mafVMEImage,"Generic VME storing vtkImageData datasets");
  mafPlugNodeMacro(mafVMEMesh,"Generic VME storing vtkUnstructuredGrid datasets");
  mafPlugNodeMacro(mafVMESurface,"Generic VME storing vtkPolyData surfaces datasets");
  mafPlugNodeMacro(mafVMESurfaceParametric,"Generic VME storing vtkPolyData parametric surfaces");
  mafPlugNodeMacro(mafVMEVolumeGray,"Generic VME storing vtk volume datasets with one scalar component");
  mafPlugNodeMacro(mafVMEVolumeRGB,"Generic VME storing vtk volume datasets with 3 RGB scalar components");
  mafPlugNodeMacro(mafVMEPolyline,"Generic VME storing polyline datasets");
  mafPlugNodeMacro(mafVMEPolylineSpline,"Generic VME outputting a spline");
  mafPlugNodeMacro(mafVMEMeter,"Generic VME generating meter");
  mafPlugNodeMacro(mafVMEInfoText,"Generic VME for text information in gui");
  mafPlugNodeMacro(mafVMELandmark,"VME representing a marker");
  mafPlugNodeMacro(mafVMELandmarkCloud,"VME representing a cloud of mafVMELandmark");
  mafPlugNodeMacro(mafVMEProber,"VME representing a prober");
  //mafPlugNodeMacro(mafVMEGizmo,"VME representing a tool");
  mafPlugNodeMacro(mafVMEPointSet,"VME representing a set of points");
  mafPlugNodeMacro(mafVMERefSys,"VME representing a reference system");
  mafPlugNodeMacro(mafVMEScalar,"VME representing time varying scalar values");
#ifdef MAF_USE_ITK
  mafPlugNodeMacro(mafVMEScalarMatrix,"VME representing time varying scalar values");
#endif
  mafPlugNodeMacro(mafVMESlicer,"VME representing a slice of a volume");
  mafPlugNodeMacro(mafVMEVector,"VME representing aa applyed vector");


  //mafPlugObjectMacro(mmaObject,"Object attributes");
  mafPlugObjectMacro(mafAttributeSegmentationVolume,"Segmentation Volume attributes");

  mafPlugNodeMacro(mafVMEWrappedMeter,"Generalized VME Meter with wrapping geometry");
  mafPlugNodeMacro(mafVMEPolylineGraph,"VME for Graph and Polyline");
  mafPlugNodeMacro(mafVMEPolylineEditor,"VME for Editing Graph and Polyline");
  mafPlugNodeMacro(mafVMESurfaceEditor,"VME for Editing Surface");
  mafPlugNodeMacro(mafVMELabeledVolume,"VME representing a label put on a volume");
  mafPlugNodeMacro(mafVMEMaps, "VME representing density-distace surface scalars");
  mafPlugNodeMacro(mafVMESegmentationVolume, "VME for Segmented Volume");

#ifdef MAF_USE_ITK
  mafPlugNodeMacro(mafVMERawMotionData,"VME that is a group for RawMotionData");
  mafPlugNodeMacro(mafVMEAnalog,"VME rapresenting EMG scalar data");
  mafPlugNodeMacro(mafVMEStent,"VME representing stent structure");
#endif
  //mafPlugNodeMacro(mafVMEthing,"VME representing a thing");

  //TODO: to be committed down
  mafPlugNodeMacro(mafVMEVolumeLarge, "VME storing large volume datasets with one scalar component");
}

//------------------------------------------------------------------------------
const char* mafVMEFactory::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafVMEFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF VMEs";
}

//------------------------------------------------------------------------------
mafVME *mafVMEFactory::CreateVMEInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(Superclass::CreateInstance(type_name));
}
//------------------------------------------------------------------------------
mafVMEFactory* mafVMEFactory::GetInstance()
//------------------------------------------------------------------------------
{
  static mafVMEFactory &istance = *(mafVMEFactory::New());
  Initialize();
  return &istance;
}
// //------------------------------------------------------------------------------
// void mafVMEFactory::RegisterNewNode(const char* node_name, const char* description, mafCreateObjectFunction createFunction)
// //------------------------------------------------------------------------------
// {
//   std::vector<std::string, std::allocator<std::string> >::const_iterator it = std::find(GetNodeNames().begin (), GetNodeNames().end (), std::string(node_name));
//   if(it != GetNodeNames().end() )
//   {
//     return;
//   }
//   GetNodeNames().push_back(node_name);
//   RegisterNewObject(node_name,description,createFunction);
// }
// //------------------------------------------------------------------------------
// std::vector<std::string> & mafVMEFactory::GetNodeNames()
// //------------------------------------------------------------------------------
// {
//   static std::vector<std::string> nodeNames;
//   return nodeNames;
// }