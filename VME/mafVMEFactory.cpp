/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-08 14:08:39 $
  Version:   $Revision: 1.15 $
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

#include "mafVMEFactory.h"

#include "mafVersion.h"

#include "mafVMEGeneric.h"
#include "mafVMEGizmo.h"
#include "mafVMEGroup.h"
#include "mafVMEImage.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEMeter.h"
#include "mafVMEPointSet.h"
#include "mafVMEPolyline.h"
#include "mafVMEProber.h"
#include "mafVMERefSys.h"
#include "mafVMERoot.h"
#include "mafVMEScalar.h"
#include "mafVMESlicer.h"
#include "mafVMESurface.h"
#include "mafVMEVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"

#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mmaMeter.h"
#include "mafTagArray.h"
#include "mafVMEItemVTK.h"
#include "mafVMEItemScalar.h"

#include "mafIndent.h"
#include <string>
#include <ostream>

mafCxxTypeMacro(mafVMEFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafVMEFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=mafVMEFactory::New();

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
mafVMEFactory::mafVMEFactory()
//------------------------------------------------------------------------------
{
  //
  // Plug here Nodes in this factory
  //

  mafPlugObjectMacro(mafTagArray,"a basic kind of attribute used to store key-value pairs");
  mafPlugObjectMacro(mafVMEItemVTK,"a VME item storing VTK datasets");
  mafPlugObjectMacro(mafVMEItemScalar,"a VME item storing scalar data through vnl matrix");
  mafPlugObjectMacro(mmaMeter,"Meter attributes");
  mafPlugObjectMacro(mmaMaterial,"Material attributes");
  mafPlugObjectMacro(mmaVolumeMaterial,"Volume material attributes");
  
  mafPlugNodeMacro(mafVMERoot,"root for VME tree");
  mafPlugNodeMacro(mafVMEGroup,"VME used for composing assemblies");
  mafPlugNodeMacro(mafVMEGeneric,"Generic VME storing VTK datasets");
  mafPlugNodeMacro(mafVMEImage,"Generic VME storing vtkImageData datasets");
  mafPlugNodeMacro(mafVMESurface,"Generic VME storing vtkPolyData surfaces datasets");
  mafPlugNodeMacro(mafVMEVolumeGray,"Generic VME storing vtk volume datasets with one scalar component");
  mafPlugNodeMacro(mafVMEVolumeRGB,"Generic VME storing vtk volume datasets with 3 RGB scalar components");
  mafPlugNodeMacro(mafVMEPolyline,"Generic VME storing polyline datasets");
  mafPlugNodeMacro(mafVMEMeter,"Generic VME generating meter");
  mafPlugNodeMacro(mafVMELandmark,"VME representing a marker");
  mafPlugNodeMacro(mafVMELandmarkCloud,"VME representing a cloud of mafVMELandmark");
  mafPlugNodeMacro(mafVMEProber,"VME representing a prober");
  //mafPlugNodeMacro(mafVMEGizmo,"VME representing a tool");
  mafPlugNodeMacro(mafVMEPointSet,"VME representing a set of points");
  mafPlugNodeMacro(mafVMERefSys,"VME representing a reference system");
  mafPlugNodeMacro(mafVMEScalar,"VME representing time varying scalar values");
  mafPlugNodeMacro(mafVMESlicer,"VME representing a slice of a volume");
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
