/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEProber.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-14 09:55:56 $
  Version:   $Revision: 1.5 $
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
#include "mmgGui.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafStorageElement.h"
#include "mafDataPipeCustom.h"
#include "mafVMEOutputSurface.h"
#include "mmaMaterial.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkDistanceFilter.h"

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
  
  m_VMEVolumeAccept  = new mafVMEVolumeAccept();
  m_VMESurfaceAccept = new mafVMESurfaceAccept();

  vtkNEW(m_Normals);
  vtkNEW(m_Prober);
  m_Prober->SetInput((vtkDataSet *)m_Normals->GetOutput());
//  m_Prober->SetSource(NULL);

  mafNEW(m_Transform);
  mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a data pipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  dpipe->SetInput(m_Prober->GetOutput());
  SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
mafVMEProber::~mafVMEProber()
//-------------------------------------------------------------------------
{
  // these links are children, thus it's not our responsibility to
  // destroy them, it's part of the vtkTree one's
  mafDEL(m_Transform);
  vtkDEL(m_Normals);
  vtkDEL(m_Prober);
  cppDEL(m_VMEVolumeAccept);
  cppDEL(m_VMESurfaceAccept);
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
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  mafVME *surf = mafVME::SafeDownCast(GetSurfaceLink());
  
  if(vol && surf)
  {
    vol->GetOutput()->Update();
    surf->GetOutput()->Update();
    if (vtkDataSet *vtkdata = vol->GetOutput()->GetVTKData())
    {
      if(vol->GetOutput()->GetVTKData() && surf->GetOutput()->GetVTKData())
      {
        m_Normals->SetInput((vtkPolyData *)surf->GetOutput()->GetVTKData());
        m_Normals->ComputePointNormalsOn();
        m_Normals->SplittingOff();
        m_Normals->Update();

//        m_Prober->SetInput((vtkDataSet *)m_Normals->GetOutput());
        m_Prober->SetSource(vol->GetOutput()->GetVTKData());
        if(GetMode() == mafVMEProber::DENSITY_MODE)
          m_Prober->SetFilterModeToDensity();
        else
        {
          m_Prober->SetFilterModeToDistance();
          m_Prober->SetThreshold(GetDistanceThreshold());
          m_Prober->SetMaxDistance(GetMaxDistance());
          if(GetDistanceMode() == mafVMEProber::DISTANCE_MODE_SCALAR)
            m_Prober->SetDistanceModeToScalar();
          else
            m_Prober->SetDistanceModeToVector();
        }

        mafSmartPointer<mafTransformFrame> maps_to_volume;
        maps_to_volume->SetInput(GetAbsMatrixPipe()->GetMatrixPointer());
        maps_to_volume->SetTargetFrame(vol->GetAbsMatrixPipe()->GetMatrixPointer());

        mafMatrix tmp_matrix = maps_to_volume->GetMatrix();

        m_Prober->SetInputMatrix(tmp_matrix.GetVTKMatrix());
      }
    }
  }
}

//-----------------------------------------------------------------------
void mafVMEProber::InternalUpdate()
//-----------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
int mafVMEProber::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEProber *prober = mafVMEProber::SafeDownCast(a);
    mafNode *linked_node = prober->GetLink("Volume");
    if (linked_node)
    {
      this->SetLink("Volume", linked_node);
    }
    linked_node = prober->GetLink("Surface");
    if (linked_node)
    {
      this->SetLink("Surface", linked_node);
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
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEProber::SetSurfaceLink(mafNode *surface)
//-----------------------------------------------------------------------
{
  SetLink("Surface", surface);
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEProber::SetModeToDensity()
//-----------------------------------------------------------------------
{
  this->SetMode(mafVMEProber::DENSITY_MODE);
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEProber::SetModeToDistance()
//-----------------------------------------------------------------------
{
  this->SetMode(mafVMEProber::DISTANCE_MODE);
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
  if (mode != mafVMEProber::DENSITY_MODE && mode != mafVMEProber::DISTANCE_MODE)
  {
    mafErrorMacro("trying to set the map mode to invalid type: allowed types are DENSITY_MODE & DISTANCE_MODE");
    return;
  }

  m_ProberMode = mode;
  GetTagArray()->SetTag(mafTagItem("MFL_MAP_MODE",m_ProberMode));
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
/*  if (thr < 0)
  {
    mafErrorMacro("Bad initial distance threshold value.");
    return;
  }
*/
  m_DistThreshold = thr;

  GetTagArray()->SetTag(mafTagItem("MFL_MAP_DISTANCE_THRESHOLD", m_DistThreshold));
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
  if (mode != mafVMEProber::DISTANCE_MODE_SCALAR && mode != mafVMEProber::DISTANCE_MODE_VECTOR)
  {
    mafErrorMacro("trying to set the map distance mode to invalid type: allowed types are DISTANCE_MODE_SCALAR & DISTANCE_MODE_VECTOR");
    return;
  }

  m_DistanceModeType = mode;
  GetTagArray()->SetTag(mafTagItem("MFL_MAP_DISTANCE_MODE", m_DistanceModeType));
  this->Modified();
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
  this->Modified();
}
//-------------------------------------------------------------------------
float mafVMEProber::GetLowDensity()
//-------------------------------------------------------------------------
{
  return mafRestoreNumericFromTag(GetTagArray(),"MFL_MAP_LOW_DENSITY",m_LowDensity,-1.0f,300.0f);
}
//-------------------------------------------------------------------------
mmgGui* mafVMEProber::CreateGui()
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
        e->SetArg((long)m_VMEVolumeAccept);
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
        e->SetArg((long)m_VMESurfaceAccept);
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
