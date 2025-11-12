/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEFactory
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

#include "albaVMEFactory.h"

#include "albaVMEExternalData.h"
#include "albaVMEGeneric.h"
#include "albaVMEGizmo.h"
#include "albaVMEGroup.h"
#include "albaVMEImage.h"
#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEMeter.h"
#include "albaVMEInfoText.h"
#include "albaVMEMesh.h"
#include "albaVMEPointSet.h"
#include "albaVMEPolyline.h"
#include "albaVMEPolylineSpline.h"
#include "albaVMEProber.h"
#include "albaVMERefSys.h"
#include "albaVMERoot.h"

#ifdef ALBA_USE_ITK
  #include "albaVMEScalarMatrix.h"
  #include "albaVMEItemScalarMatrix.h"
#endif

#include "albaVMEScalar.h"
#include "albaVMESlicer.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEVector.h"
#include "albaVMEVolume.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEPointCloud.h"

#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mmaApplicationLayout.h"
#include "albaAttributeTraceability.h"
#include "mmaMeter.h"
#include "albaTagArray.h"
#include "albaVMEItemVTK.h"


#include "albaVMEWrappedMeter.h"
#include "albaVMEPolylineGraph.h"
#include "albaVMEPolylineEditor.h"
#include "albaVMEMaps.h"

#include "albaVMESurfaceEditor.h"
#include "albaVMELabeledVolume.h"
#include "albaVMESegmentationVolume.h"
#include "albaAttributeSegmentationVolume.h"

#ifdef ALBA_USE_ITK
#include "albaVMERawMotionData.h"
#include "albaVMEAnalog.h"
#include "albaVMEStent.h"
#endif


#include "albaIndent.h"
#include <string>
#include <ostream>


albaCxxTypeMacro(albaVMEFactory);

bool albaVMEFactory::m_Initialized=false;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int albaVMEFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    // m_Instance=albaVMEFactory::New();

    m_Initialized=true;
    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      return ALBA_OK;  
    }
    else
    {
      m_Initialized=true;
      return ALBA_ERROR;
    }
  }
  
  return ALBA_OK;
}

//------------------------------------------------------------------------
albaVMEFactory::albaVMEFactory()
//------------------------------------------------------------------------------
{
  //
  // Plug here Nodes in this factory
  //
  albaPlugObjectMacro(albaTagArray,"a basic kind of attribute used to store key-value pairs");
  albaPlugObjectMacro(albaVMEItemVTK,"a VME item storing VTK datasets");
#ifdef ALBA_USE_ITK
  albaPlugObjectMacro(albaVMEItemScalarMatrix,"a VME item storing matrix scalar data through vnl matrix");
#endif
  albaPlugObjectMacro(mmaMeter,"Meter attributes");
  albaPlugObjectMacro(mmaMaterial,"Material attributes");
  albaPlugObjectMacro(mmaVolumeMaterial,"Volume material attributes");
  albaPlugObjectMacro(mmaApplicationLayout,"Applycation layout attributes");
  albaPlugObjectMacro(albaAttributeTraceability,"Trial event attributes");
  
  albaPlugVMEMacro(albaVMERoot,"Root");
  albaPlugVMEMacro(albaVMEExternalData,"External data.");
  albaPlugVMEMacro(albaVMEGroup,"Group");
  albaPlugVMEMacro(albaVMEGeneric,"Generic");
  albaPlugVMEMacro(albaVMEImage,"Image");
  albaPlugVMEMacro(albaVMEMesh,"Mesh");
  albaPlugVMEMacro(albaVMESurface,"Surface");
	albaPlugVMEMacro(albaVMEPointCloud, "Point Cloud");
  albaPlugVMEMacro(albaVMESurfaceParametric,"Parametric Surface");
  albaPlugVMEMacro(albaVMEVolumeGray,"Volume");
  albaPlugVMEMacro(albaVMEVolumeRGB,"Volume RGB");
  albaPlugVMEMacro(albaVMEPolyline,"Polyline");
  albaPlugVMEMacro(albaVMEPolylineSpline,"Spline");
	albaPlugVMEMacro(albaVMEMeter, "Meter");
  albaPlugVMEMacro(albaVMEInfoText,"Text");
  albaPlugVMEMacro(albaVMELandmark,"Landmark");
  albaPlugVMEMacro(albaVMELandmarkCloud,"Landmark Cloud");
  albaPlugVMEMacro(albaVMEProber,"Prober");
  //albaPlugVMEMacro(albaVMEGizmo,"VME representing a tool");
  albaPlugVMEMacro(albaVMEPointSet,"Point set");
  albaPlugVMEMacro(albaVMERefSys,"Reference System");
  albaPlugVMEMacro(albaVMEScalar,"Scalar");
#ifdef ALBA_USE_ITK
  albaPlugVMEMacro(albaVMEScalarMatrix,"Scalar Matrix");
#endif
  albaPlugVMEMacro(albaVMESlicer,"Slicer");
  albaPlugVMEMacro(albaVMEVector,"Vector");


  //albaPlugObjectMacro(mmaObject,"Object attributes");
  albaPlugObjectMacro(albaAttributeSegmentationVolume,"Segmentation Volume attributes");

  albaPlugVMEMacro(albaVMEWrappedMeter,"Wrapped Meter");
  albaPlugVMEMacro(albaVMEPolylineGraph,"Polyline Graph");
  albaPlugVMEMacro(albaVMEPolylineEditor,"Polyline Editor");
  albaPlugVMEMacro(albaVMESurfaceEditor,"Surface Editor");
  albaPlugVMEMacro(albaVMELabeledVolume,"Labeled Volume");
  albaPlugVMEMacro(albaVMEMaps, "VME representing density-distace surface scalars");
  albaPlugVMEMacro(albaVMESegmentationVolume, "Segmented Volume");

#ifdef ALBA_USE_ITK
  albaPlugVMEMacro(albaVMERawMotionData,"Raw Motion Data");
  albaPlugVMEMacro(albaVMEAnalog,"EMG scalar data");
  albaPlugVMEMacro(albaVMEStent,"Stent");
#endif

}

//------------------------------------------------------------------------------
const char* albaVMEFactory::GetDescription() const
{
  return "Factory for ALBA VMEs";
}

//------------------------------------------------------------------------------
albaVME *albaVMEFactory::CreateVMEInstance(const char *type_name)
{
  return albaVME::SafeDownCast(Superclass::CreateInstance(type_name));
}
//------------------------------------------------------------------------------
void albaVMEFactory::RegisterNewVME(const char* node_name, const char* typeName, albaCreateObjectFunction createFunction)
{
	std::vector<std::string, std::allocator<std::string> >::const_iterator it = std::find(GetNodeNames().begin(), GetNodeNames().end(), std::string(node_name));
	if (it != GetNodeNames().end())
	{
		return;
	}
	GetNodeNames().push_back(node_name);
	RegisterNewObject(node_name, typeName, createFunction);
}
//------------------------------------------------------------------------------
albaVMEFactory* albaVMEFactory::GetInstance()
{
	static albaVMEFactory &istance = *(albaVMEFactory::New());
	Initialize();
	return &istance;
}
//------------------------------------------------------------------------------
std::vector<std::string> & albaVMEFactory::GetNodeNames()
{
	static std::vector<std::string> nodeNames;
	return nodeNames;
}
