/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEProber
 Authors: Paolo Quadrani
 
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

#include "albaVMEProber.h"
#include "albaGUI.h"
#include "mmaMaterial.h"

#include "albaTransform.h"
#include "albaTransformFrame.h"
#include "albaStorageElement.h"
#include "albaDataPipeCustomProber.h"
#include "albaVMEOutputSurface.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaMatrix.h"
#include "albaAbsMatrixPipe.h"


#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkTransformPolyDataFilter.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEProber)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEProber::albaVMEProber()
//-------------------------------------------------------------------------
{
	m_DistThreshold   = -1.0;
	m_MaxDistance     = -1.0;
	m_DistanceModeType= -1;
	
  m_ProberMode  = -1;
  m_HighDensity = -1.0;
  m_LowDensity  = -1.0;
  
  albaNEW(m_Transform);
  albaVMEOutputSurface *output = albaVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and ALBA
  m_ProbingDataPipe = albaDataPipeCustomProber::New();
  m_ProbingDataPipe->SetDependOnAbsPose(true);
  m_ProbingDataPipe->SetMode(GetMode());
  m_ProbingDataPipe->SetDistanceThreshold(GetDistanceThreshold());
  m_ProbingDataPipe->SetMaxDistance(GetMaxDistance());
  m_ProbingDataPipe->SetDistanceMode(GetDistanceMode());
  m_ProbingDataPipe->SetHighDensity(GetHighDensity());
  m_ProbingDataPipe->SetLowDensity(GetLowDensity());
  SetDataPipe(m_ProbingDataPipe);
}

//-------------------------------------------------------------------------
albaVMEProber::~albaVMEProber()
//-------------------------------------------------------------------------
{
  // these links are children, thus it's not our responsibility to
  // destroy them, it's part of the vtkTree one's
  albaDEL(m_Transform);
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMEProber::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
  }
  return material;
}

//-------------------------------------------------------------------------
int albaVMEProber::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==ALBA_OK)
  {
    // force material allocation
    GetMaterial();
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
void albaVMEProber::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  albaVME *surf = GetSurfaceLink();
  albaVME *vol = GetVolumeLink();
  
  m_ProbingDataPipe->SetSurface(surf);
  m_ProbingDataPipe->SetVolume(vol);

  m_SurfaceName = surf ? surf->GetName() : _("none");
  m_VolumeName = vol ? vol->GetName() : _("none");

  if (m_Gui)
  {
    m_Gui->Update();
  }
}

//-------------------------------------------------------------------------
int albaVMEProber::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMEProber *prober = albaVMEProber::SafeDownCast(a);
    albaVME *volume_linked_node = prober->GetLink("Volume");
    if (volume_linked_node)
    {
      SetVolumeLink(volume_linked_node);
    }
    albaVME *surface_linked_node = prober->GetLink("Surface");
    if (surface_linked_node)
    {
      SetSurfaceLink(surface_linked_node);
    }
    m_Transform->SetMatrix(prober->m_Transform->GetMatrix());
    m_ProberMode      = prober->m_ProberMode;
    m_DistThreshold   = prober->m_DistThreshold;
    m_MaxDistance     = prober->m_MaxDistance;
    m_DistanceModeType= prober->m_DistanceModeType;
    m_HighDensity     = prober->m_HighDensity;
    m_LowDensity      = prober->m_LowDensity;
    m_VolumeName      = prober->m_VolumeName;
    m_SurfaceName     = prober->m_SurfaceName;
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
bool albaVMEProber::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((albaVMEProber *)vme)->m_Transform->GetMatrix() && \
      GetLink("Volume") == ((albaVMEProber *)vme)->GetLink("Volume") && \
      GetLink("Surface")== ((albaVMEProber *)vme)->GetLink("Surface") && \
      m_ProberMode      == ((albaVMEProber *)vme)->m_ProberMode && \
      m_DistThreshold   == ((albaVMEProber *)vme)->m_DistThreshold && \
      m_MaxDistance     == ((albaVMEProber *)vme)->m_MaxDistance && \
      m_DistanceModeType== ((albaVMEProber *)vme)->m_DistanceModeType && \
      m_HighDensity     == ((albaVMEProber *)vme)->m_HighDensity && \
      m_LowDensity      == ((albaVMEProber *)vme)->m_LowDensity && \
      m_VolumeName      == ((albaVMEProber *)vme)->m_VolumeName && \
      m_SurfaceName     == ((albaVMEProber *)vme)->m_SurfaceName;
  }
  return ret;
}

//-------------------------------------------------------------------------
void albaVMEProber::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void albaVMEProber::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-------------------------------------------------------------------------
bool albaVMEProber::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool albaVMEProber::IsDataAvailable()
//-------------------------------------------------------------------------
{
  albaVME *vol = GetVolumeLink();
  albaVME *surf = GetSurfaceLink();
  return (vol && surf && vol->IsDataAvailable() && surf->IsDataAvailable());
}

//-----------------------------------------------------------------------
albaVME *albaVMEProber::GetVolumeLink()
//-----------------------------------------------------------------------
{
  return GetLink("Volume");
}
//-----------------------------------------------------------------------
albaVME *albaVMEProber::GetSurfaceLink()
//-----------------------------------------------------------------------
{
  return GetLink("Surface");
}
//-----------------------------------------------------------------------
void albaVMEProber::SetVolumeLink(albaVME *volume)
//-----------------------------------------------------------------------
{
  SetLink("Volume", volume);
  m_ProbingDataPipe->SetVolume(volume);
  Modified();
}
//-----------------------------------------------------------------------
void albaVMEProber::SetSurfaceLink(albaVME *surface)
//-----------------------------------------------------------------------
{
  SetLink("Surface", surface);
  m_ProbingDataPipe->SetSurface(surface);
  Modified();
}
//-----------------------------------------------------------------------
void albaVMEProber::SetModeToDensity()
//-----------------------------------------------------------------------
{
  this->SetMode(albaDataPipeCustomProber::DENSITY_MODE);
  m_ProbingDataPipe->SetModeToDensity();
  Modified();
}
//-----------------------------------------------------------------------
void albaVMEProber::SetModeToDistance()
//-----------------------------------------------------------------------
{
  this->SetMode(albaDataPipeCustomProber::DISTANCE_MODE);
  m_ProbingDataPipe->SetModeToDistance();
  Modified();
}
//-----------------------------------------------------------------------
int albaVMEProber::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMEProber::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
      m_Transform->SetMatrix(matrix);
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
void albaVMEProber::SetMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != albaDataPipeCustomProber::DENSITY_MODE && mode != albaDataPipeCustomProber::DISTANCE_MODE)
  {
    albaErrorMacro("trying to set the map mode to invalid type: allowed types are DENSITY_MODE & DISTANCE_MODE");
    return;
  }

  m_ProberMode = mode;
  GetTagArray()->SetTag(albaTagItem("MFL_MAP_MODE",m_ProberMode));
  m_ProbingDataPipe->SetMode(m_ProberMode);
  this->Modified();
}
//-------------------------------------------------------------------------
int albaVMEProber::GetMode()
//-------------------------------------------------------------------------
{
  return (int)albaRestoreNumericFromTag(GetTagArray(), "MFL_MAP_MODE",m_ProberMode,-1,0);
}
//-------------------------------------------------------------------------
void albaVMEProber::SetDistanceThreshold(float thr)
//-------------------------------------------------------------------------
{
  m_DistThreshold = thr;

  GetTagArray()->SetTag(albaTagItem("MFL_MAP_DISTANCE_THRESHOLD", m_DistThreshold));
  m_ProbingDataPipe->SetDistanceThreshold(m_DistThreshold);
  this->Modified();
}
//-------------------------------------------------------------------------
float albaVMEProber::GetDistanceThreshold()
//-------------------------------------------------------------------------
{
  return albaRestoreNumericFromTag(GetTagArray(),"MFL_MAP_DISTANCE_THRESHOLD", m_DistThreshold, -1.0f, 0.0f);
}
//-------------------------------------------------------------------------
void albaVMEProber::SetMaxDistance(float max_dist)
//-------------------------------------------------------------------------
{
  if (max_dist < 0)
  {
    albaErrorMacro("Bad max distance value.");
    return;
  }

  m_MaxDistance = max_dist;

  GetTagArray()->SetTag(albaTagItem("MFL_MAP_MAX_DISTANCE", m_MaxDistance));
  m_ProbingDataPipe->SetMaxDistance(m_MaxDistance);
  this->Modified();
}
//-------------------------------------------------------------------------
float albaVMEProber::GetMaxDistance()
//-------------------------------------------------------------------------
{
  return albaRestoreNumericFromTag(GetTagArray(),"MFL_MAP_MAX_DISTANCE",m_MaxDistance,-1.0f, 1.0f);
}
//-------------------------------------------------------------------------
void albaVMEProber::SetDistanceMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != albaDataPipeCustomProber::DISTANCE_MODE_SCALAR && mode != albaDataPipeCustomProber::DISTANCE_MODE_VECTOR)
  {
    albaErrorMacro("trying to set the map distance mode to invalid type: allowed types are DISTANCE_MODE_SCALAR & DISTANCE_MODE_VECTOR");
    return;
  }

  m_DistanceModeType = mode;
  GetTagArray()->SetTag(albaTagItem("MFL_MAP_DISTANCE_MODE", m_DistanceModeType));
  m_ProbingDataPipe->SetDistanceMode(m_DistanceModeType);
  this->Modified();
}
//-------------------------------------------------------------------------
void albaVMEProber::SetDistanceModeToScalar()
//-------------------------------------------------------------------------
{
  this->SetDistanceMode(albaDataPipeCustomProber::DISTANCE_MODE_SCALAR);
}
//-------------------------------------------------------------------------
void albaVMEProber::SetDistanceModeToVector()
//-------------------------------------------------------------------------
{
  this->SetDistanceMode(albaDataPipeCustomProber::DISTANCE_MODE_VECTOR);
}
//-------------------------------------------------------------------------
int albaVMEProber::GetDistanceMode()
//-------------------------------------------------------------------------
{
  return albaRestoreNumericFromTag(GetTagArray(),"MFL_MAP_DISTANCE_MODE",m_DistanceModeType,-1,0);
}
//-------------------------------------------------------------------------
void albaVMEProber::SetHighDensity(float high_dens)
//-------------------------------------------------------------------------
{
  m_HighDensity = high_dens;

  GetTagArray()->SetTag(albaTagItem("MFL_MAP_HIGH_DENSITY", m_HighDensity));
  m_ProbingDataPipe->SetHighDensity(m_HighDensity);
  this->Modified();
}
//-------------------------------------------------------------------------
float albaVMEProber::GetHighDensity()
//-------------------------------------------------------------------------
{
  return albaRestoreNumericFromTag(GetTagArray(),"MFL_MAP_HIGH_DENSITY",m_HighDensity,-1.0f,600.0f);
}
//-------------------------------------------------------------------------
void albaVMEProber::SetLowDensity(float low_dens)
//-------------------------------------------------------------------------
{
  m_LowDensity = low_dens;

  GetTagArray()->SetTag(albaTagItem("MFL_MAP_LOW_DENSITY", m_LowDensity));
  m_ProbingDataPipe->SetLowDensity(m_LowDensity);
  this->Modified();
}
//-------------------------------------------------------------------------
float albaVMEProber::GetLowDensity()
//-------------------------------------------------------------------------
{
  return albaRestoreNumericFromTag(GetTagArray(),"MFL_MAP_LOW_DENSITY",m_LowDensity,-1.0f,300.0f);
}
//-------------------------------------------------------------------------
albaGUI* albaVMEProber::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  albaVME *vol = GetVolumeLink();
  m_VolumeName = vol ? vol->GetName() : _("none");
  m_Gui->Button(ID_VOLUME_LINK,&m_VolumeName,_("Volume"), _("Select the volume to be probed"));

  albaVME *surf = GetSurfaceLink();
  m_SurfaceName = surf ? surf->GetName() : _("none");
  m_Gui->Button(ID_SURFACE_LINK,&m_SurfaceName,_("Surface"), _("Select the polydata to probe the volume"));

  m_ProberMode = GetMode();
  wxString prober_mode[2] = {_("Density"), _("Distance")};
  m_Gui->Combo(ID_MODALITY,_("Modality"), &m_ProberMode, 2, prober_mode);
	m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEProber::OnEvent(albaEventBase *alba_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_VOLUME_LINK:
      {
        albaString title = _("Choose volume vme");
        e->SetId(VME_CHOOSE);
        e->SetPointer(&albaVMEProber::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        albaVME *n = e->GetVme();
        if (n != NULL)
        {
          SetVolumeLink(n);
          m_VolumeName = n->GetName();
          m_Gui->Update();
        }
      }
      break;
      case ID_SURFACE_LINK:
      {
        albaString title = _("Choose surface vme");
        e->SetId(VME_CHOOSE);
        e->SetPointer(&albaVMEProber::OutputSurfaceAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        albaVME *n = e->GetVme();
        if (n != NULL)
        {
          SetSurfaceLink(n);
          m_SurfaceName = n->GetName();
          m_Gui->Update();
        }
      }
      break;
      case ID_MODALITY:
        SetMode(m_ProberMode);
      break;
      default:
      albaVME::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
char** albaVMEProber::GetIcon() 
//-------------------------------------------------------------------------
{
#include "albaVMEProber.xpm"
  return albaVMEProber_xpm;
}
