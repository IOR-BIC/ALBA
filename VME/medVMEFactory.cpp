/*=========================================================================

 Program: MAF2
 Module: medVMEFactory
 Authors: Daniele Giunchi
 
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

#include "medVMEFactory.h"

#include "mafVersion.h"

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

mafCxxTypeMacro(medVMEFactory);

bool medVMEFactory::m_Initialized=false;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int medVMEFactory::Initialize()
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
      m_Initialized=true;
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
const char* medVMEFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for Medical VMEs";
}

//------------------------------------------------------------------------------
mafVME *medVMEFactory::CreateVMEInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(Superclass::CreateInstance(type_name));
}
//------------------------------------------------------------------------------
medVMEFactory* medVMEFactory::GetInstance()
//------------------------------------------------------------------------------
{
  static medVMEFactory &istance = *(medVMEFactory::New());
  Initialize();
  return &istance;
}