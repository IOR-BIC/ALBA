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
  
  mafPlugVMEMacro(mafVMERoot,"Root");
  mafPlugVMEMacro(mafVMEExternalData,"External data.");
  mafPlugVMEMacro(mafVMEGroup,"Group");
  mafPlugVMEMacro(mafVMEGeneric,"Generic");
  mafPlugVMEMacro(mafVMEImage,"Image");
  mafPlugVMEMacro(mafVMEMesh,"Mesh");
  mafPlugVMEMacro(mafVMESurface,"Surface");
  mafPlugVMEMacro(mafVMESurfaceParametric,"Parametric Surface");
  mafPlugVMEMacro(mafVMEVolumeGray,"Volume");
  mafPlugVMEMacro(mafVMEVolumeRGB,"Volume RGB");
  mafPlugVMEMacro(mafVMEPolyline,"Polyline");
  mafPlugVMEMacro(mafVMEPolylineSpline,"Spline");
	mafPlugVMEMacro(mafVMEMeter, "Meter");
  mafPlugVMEMacro(mafVMEInfoText,"Text");
  mafPlugVMEMacro(mafVMELandmark,"Landmark");
  mafPlugVMEMacro(mafVMELandmarkCloud,"Landmark Cloud");
  mafPlugVMEMacro(mafVMEProber,"Prober");
  //mafPlugVMEMacro(mafVMEGizmo,"VME representing a tool");
  mafPlugVMEMacro(mafVMEPointSet,"Point set");
  mafPlugVMEMacro(mafVMERefSys,"Reference System");
  mafPlugVMEMacro(mafVMEScalar,"Scalar");
#ifdef MAF_USE_ITK
  mafPlugVMEMacro(mafVMEScalarMatrix,"Scalar Matrix");
#endif
  mafPlugVMEMacro(mafVMESlicer,"Slicer");
  mafPlugVMEMacro(mafVMEVector,"Vector");


  //mafPlugObjectMacro(mmaObject,"Object attributes");
  mafPlugObjectMacro(mafAttributeSegmentationVolume,"Segmentation Volume attributes");

  mafPlugVMEMacro(mafVMEWrappedMeter,"Wrapped Meter");
  mafPlugVMEMacro(mafVMEPolylineGraph,"Polyline Graph");
  mafPlugVMEMacro(mafVMEPolylineEditor,"Polyline Editor");
  mafPlugVMEMacro(mafVMESurfaceEditor,"Surface Editor");
  mafPlugVMEMacro(mafVMELabeledVolume,"Labeled Volume");
  mafPlugVMEMacro(mafVMEMaps, "VME representing density-distace surface scalars");
  mafPlugVMEMacro(mafVMESegmentationVolume, "Segmented Volume");

#ifdef MAF_USE_ITK
  mafPlugVMEMacro(mafVMERawMotionData,"Raw Motion Data");
  mafPlugVMEMacro(mafVMEAnalog,"EMG scalar data");
  mafPlugVMEMacro(mafVMEStent,"Stent");
#endif

}

//------------------------------------------------------------------------------
const char* mafVMEFactory::GetMAFSourceVersion() const
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafVMEFactory::GetDescription() const
{
  return "Factory for MAF VMEs";
}

//------------------------------------------------------------------------------
mafVME *mafVMEFactory::CreateVMEInstance(const char *type_name)
{
  return mafVME::SafeDownCast(Superclass::CreateInstance(type_name));
}
//------------------------------------------------------------------------------
void mafVMEFactory::RegisterNewVME(const char* node_name, const char* typeName, mafCreateObjectFunction createFunction)
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
mafVMEFactory* mafVMEFactory::GetInstance()
{
	static mafVMEFactory &istance = *(mafVMEFactory::New());
	Initialize();
	return &istance;
}
//------------------------------------------------------------------------------
std::vector<std::string> & mafVMEFactory::GetNodeNames()
{
	static std::vector<std::string> nodeNames;
	return nodeNames;
}
