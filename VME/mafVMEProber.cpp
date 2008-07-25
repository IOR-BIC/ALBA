/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEProber.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:06:00 $
  Version:   $Revision: 1.13 $
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

#include "mafVMEProber.h"
#include "mafGUI.h"
#include "mmaMaterial.h"

#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafStorageElement.h"
#include "mafDataPipeCustomProber.h"
#include "mafVMEOutputSurface.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h"


#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkTransformPolyDataFilter.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEProber)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEProber::mafVMEProber()
//-------------------------------------------------------------------------
{
	m_DistThreshold   = -1.0;
	m_MaxDistance     = -1.0;
	m_DistanceModeType= -1;
	
  m_ProberMode  = -1;
  m_HighDensity = -1.0;
  m_LowDensity  = -1.0;
  
  mafNEW(m_Transform);
  mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and MAF
  m_ProbingDataPipe = mafDataPipeCustomProber::New();
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
mafVMEProber::~mafVMEProber()
//-------------------------------------------------------------------------
{
  // these links are children, thus it's not our responsibility to
  // destroy them, it's part of the vtkTree one's
  mafDEL(m_Transform);
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEProber::GetMaterial()
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
int mafVMEProber::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mafVMEProber::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  mafVME *surf = mafVME::SafeDownCast(GetSurfaceLink());
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  
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
int mafVMEProber::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEProber *prober = mafVMEProber::SafeDownCast(a);
    mafNode *volume_linked_node = prober->GetLink("Volume");
    if (volume_linked_node)
    {
      SetVolumeLink(volume_linked_node);
    }
    mafNode *surface_linked_node = prober->GetLink("Surface");
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
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEProber::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((mafVMEProber *)vme)->m_Transform->GetMatrix() && \
      GetLink("Volume") == ((mafVMEProber *)vme)->GetLink("Volume") && \
      GetLink("Surface")== ((mafVMEProber *)vme)->GetLink("Surface") && \
      m_ProberMode      == ((mafVMEProber *)vme)->m_ProberMode && \
      m_DistThreshold   == ((mafVMEProber *)vme)->m_DistThreshold && \
      m_MaxDistance     == ((mafVMEProber *)vme)->m_MaxDistance && \
      m_DistanceModeType== ((mafVMEProber *)vme)->m_DistanceModeType && \
      m_HighDensity     == ((mafVMEProber *)vme)->m_HighDensity && \
      m_LowDensity      == ((mafVMEProber *)vme)->m_LowDensity && \
      m_VolumeName      == ((mafVMEProber *)vme)->m_VolumeName && \
      m_SurfaceName     == ((mafVMEProber *)vme)->m_SurfaceName;
  }
  return ret;
}

//-------------------------------------------------------------------------
void mafVMEProber::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void mafVMEProber::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-------------------------------------------------------------------------
bool mafVMEProber::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool mafVMEProber::IsDataAvailable()
//-------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  mafVME *surf = mafVME::SafeDownCast(GetSurfaceLink());
  return (vol && surf && vol->IsDataAvailable() && surf->IsDataAvailable());
}

//-----------------------------------------------------------------------
mafNode *mafVMEProber::GetVolumeLink()
//-----------------------------------------------------------------------
{
  return GetLink("Volume");
}
//-----------------------------------------------------------------------
mafNode *mafVMEProber::GetSurfaceLink()
//-----------------------------------------------------------------------
{
  return GetLink("Surface");
}
//-----------------------------------------------------------------------
void mafVMEProber::SetVolumeLink(mafNode *volume)
//-----------------------------------------------------------------------
{
  SetLink("Volume", volume);
  m_ProbingDataPipe->SetVolume(volume);
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEProber::SetSurfaceLink(mafNode *surface)
//-----------------------------------------------------------------------
{
  SetLink("Surface", surface);
  m_ProbingDataPipe->SetSurface(surface);
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEProber::SetModeToDensity()
//-----------------------------------------------------------------------
{
  this->SetMode(mafDataPipeCustomProber::DENSITY_MODE);
  m_ProbingDataPipe->SetModeToDensity();
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEProber::SetModeToDistance()
//-----------------------------------------------------------------------
{
  this->SetMode(mafDataPipeCustomProber::DISTANCE_MODE);
  m_ProbingDataPipe->SetModeToDistance();
  Modified();
}
//-----------------------------------------------------------------------
int mafVMEProber::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEProber::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
void mafVMEProber::SetMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != mafDataPipeCustomProber::DENSITY_MODE && mode != mafDataPipeCustomProber::DISTANCE_MODE)
  {
    mafErrorMacro("trying to set the map mode to invalid type: allowed types are DENSITY_MODE & DISTANCE_MODE");
    return;
  }

  m_ProberMode = mode;
  GetTagArray()->SetTag(mafTagItem("MFL_MAP_MODE",m_ProberMode));
  m_ProbingDataPipe->SetMode(m_ProberMode);
  this->Modified();
}
//-------------------------------------------------------------------------
int mafVMEProber::GetMode()
//-------------------------------------------------------------------------
{
  return (int)mafRestoreNumericFromTag(GetTagArray(), "MFL_MAP_MODE",m_ProberMode,-1,0);
}
//-------------------------------------------------------------------------
void mafVMEProber::SetDistanceThreshold(float thr)
//-------------------------------------------------------------------------
{
  m_DistThreshold = thr;

  GetTagArray()->SetTag(mafTagItem("MFL_MAP_DISTANCE_THRESHOLD", m_DistThreshold));
  m_ProbingDataPipe->SetDistanceThreshold(m_DistThreshold);
  this->Modified();
}
//-------------------------------------------------------------------------
float mafVMEProber::GetDistanceThreshold()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"MFL_MAP_DISTANCE_THRESHOLD", m_DistThreshold, -1.0f, 0.0f);
}
//-------------------------------------------------------------------------
void mafVMEProber::SetMaxDistance(float max_dist)
//-------------------------------------------------------------------------
{
  if (max_dist < 0)
  {
    mafErrorMacro("Bad max distance value.");
    return;
  }

  m_MaxDistance = max_dist;

  GetTagArray()->SetTag(mafTagItem("MFL_MAP_MAX_DISTANCE", m_MaxDistance));
  m_ProbingDataPipe->SetMaxDistance(m_MaxDistance);
  this->Modified();
}
//-------------------------------------------------------------------------
float mafVMEProber::GetMaxDistance()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"MFL_MAP_MAX_DISTANCE",m_MaxDistance,-1.0f, 1.0f);
}
//-------------------------------------------------------------------------
void mafVMEProber::SetDistanceMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != mafDataPipeCustomProber::DISTANCE_MODE_SCALAR && mode != mafDataPipeCustomProber::DISTANCE_MODE_VECTOR)
  {
    mafErrorMacro("trying to set the map distance mode to invalid type: allowed types are DISTANCE_MODE_SCALAR & DISTANCE_MODE_VECTOR");
    return;
  }

  m_DistanceModeType = mode;
  GetTagArray()->SetTag(mafTagItem("MFL_MAP_DISTANCE_MODE", m_DistanceModeType));
  m_ProbingDataPipe->SetDistanceMode(m_DistanceModeType);
  this->Modified();
}
//-------------------------------------------------------------------------
void mafVMEProber::SetDistanceModeToScalar()
//-------------------------------------------------------------------------
{
  this->SetDistanceMode(mafDataPipeCustomProber::DISTANCE_MODE_SCALAR);
}
//-------------------------------------------------------------------------
void mafVMEProber::SetDistanceModeToVector()
//-------------------------------------------------------------------------
{
  this->SetDistanceMode(mafDataPipeCustomProber::DISTANCE_MODE_VECTOR);
}
//-------------------------------------------------------------------------
int mafVMEProber::GetDistanceMode()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"MFL_MAP_DISTANCE_MODE",m_DistanceModeType,-1,0);
}
//-------------------------------------------------------------------------
void mafVMEProber::SetHighDensity(float high_dens)
//-------------------------------------------------------------------------
{
  m_HighDensity = high_dens;

  GetTagArray()->SetTag(mafTagItem("MFL_MAP_HIGH_DENSITY", m_HighDensity));
  m_ProbingDataPipe->SetHighDensity(m_HighDensity);
  this->Modified();
}
//-------------------------------------------------------------------------
float mafVMEProber::GetHighDensity()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"MFL_MAP_HIGH_DENSITY",m_HighDensity,-1.0f,600.0f);
}
//-------------------------------------------------------------------------
void mafVMEProber::SetLowDensity(float low_dens)
//-------------------------------------------------------------------------
{
  m_LowDensity = low_dens;

  GetTagArray()->SetTag(mafTagItem("MFL_MAP_LOW_DENSITY", m_LowDensity));
  m_ProbingDataPipe->SetLowDensity(m_LowDensity);
  this->Modified();
}
//-------------------------------------------------------------------------
float mafVMEProber::GetLowDensity()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"MFL_MAP_LOW_DENSITY",m_LowDensity,-1.0f,300.0f);
}
//-------------------------------------------------------------------------
mafGUI* mafVMEProber::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  m_VolumeName = vol ? vol->GetName() : _("none");
  m_Gui->Button(ID_VOLUME_LINK,&m_VolumeName,_("Volume"), _("Select the volume to be probed"));

  mafVME *surf = mafVME::SafeDownCast(GetSurfaceLink());
  m_SurfaceName = surf ? surf->GetName() : _("none");
  m_Gui->Button(ID_SURFACE_LINK,&m_SurfaceName,_("Surface"), _("Select the polydata to probe the volume"));

  m_ProberMode = GetMode();
  wxString prober_mode[2] = {_("density"), _("distance")};
  m_Gui->Combo(ID_MODALITY,_("modality"), &m_ProberMode, 2, prober_mode);
	m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEProber::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_VOLUME_LINK:
      {
        mafString title = _("Choose volume vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMEProber::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
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
        mafString title = _("Choose surface vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMEProber::OutputSurfaceAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
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
      mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
char** mafVMEProber::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMEProber.xpm"
  return mafVMEProber_xpm;
}
