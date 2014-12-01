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

#include "medVMEWrappedMeter.h"
#include "medVMEPolylineGraph.h"
#include "medVMEPolylineEditor.h"
#include "medVMEMaps.h"

#include "medVMESurfaceEditor.h"
#include "medVMELabeledVolume.h"
#include "medVMESegmentationVolume.h"
#include "medAttributeSegmentationVolume.h"

#ifdef MAF_USE_ITK
  #include "mafVMERawMotionData.h"
  #include "medVMEAnalog.h"
  #include "medVMEStent.h"
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
  mafPlugObjectMacro(medAttributeSegmentationVolume,"Segmentation Volume attributes");

  mafPlugNodeMacro(medVMEWrappedMeter,"Generalized VME Meter with wrapping geometry");
  mafPlugNodeMacro(medVMEPolylineGraph,"VME for Graph and Polyline");
	mafPlugNodeMacro(medVMEPolylineEditor,"VME for Editing Graph and Polyline");
 	mafPlugNodeMacro(medVMESurfaceEditor,"VME for Editing Surface");
  mafPlugNodeMacro(medVMELabeledVolume,"VME representing a label put on a volume");
  mafPlugNodeMacro(medVMEMaps, "VME representing density-distace surface scalars");
  mafPlugNodeMacro(medVMESegmentationVolume, "VME for Segmented Volume");

#ifdef MAF_USE_ITK
  mafPlugNodeMacro(mafVMERawMotionData,"VME that is a group for RawMotionData");
  mafPlugNodeMacro(medVMEAnalog,"VME rapresenting EMG scalar data");
  mafPlugNodeMacro(medVMEStent,"VME representing stent structure");
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