/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-12 11:42:51 $
  Version:   $Revision: 1.7 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewSlice.h"
#include "mafPipeVolumeSlice.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"

#include "vtkDataSet.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewSlice::mafViewSlice(wxString label, int camera_position, bool show_axes, bool show_grid, int stereo, bool external)
:mafViewVTK(label,camera_position,show_axes,show_grid,stereo,external)
//----------------------------------------------------------------------------
{
  m_CurrentVolume = NULL;
  m_AttachCamera  = NULL;
}
//----------------------------------------------------------------------------
mafViewSlice::~mafViewSlice()
//----------------------------------------------------------------------------
{
  cppDEL(m_AttachCamera);
}
//----------------------------------------------------------------------------
mafView *mafViewSlice::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewSlice *v = new mafViewSlice(m_Label, m_CameraPosition, m_ShowAxes,m_ShowGrid, m_StereoType, m_ExternalFlag);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewSlice::Create()
//----------------------------------------------------------------------------
{
  RWI_LAYERS num_layers = m_CameraPosition != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, false, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Rwi->SetAxesVisibility(m_ShowAxes != 0);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.001);
  m_Picker2D->InitializePickList();
}
//----------------------------------------------------------------------------
void mafViewSlice::CameraUpdate()
//----------------------------------------------------------------------------
{
  if (m_AttachCamera != NULL)
  {
    m_AttachCamera->UpdateCameraMatrix();
  }
  assert(m_Rwi); 
  m_Rwi->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewSlice::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->m_Pipe);

  if (pipe_name != "")
  {
    if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 1)
    {
      m_NumberOfVisibleVme = 1;
    }
    else
    {
      m_NumberOfVisibleVme++;
    }
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    pipe->SetListener(this);
    if (pipe)
    {
      if (pipe_name.Equals("mafPipeVolumeSlice"))
      {
        m_CurrentVolume = n;
        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->m_Vme);
        int slice_mode;
        vtkDataSet *data = ((mafVME *)vme)->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
        switch(m_CameraPosition)
        {
          case CAMERA_OS_X:
            slice_mode = SLICE_X;
        	break;
          case CAMERA_OS_Y:
            slice_mode = SLICE_Y;
          break;
          case CAMERA_OS_P:
            slice_mode = SLICE_ORTHO;
          break;
          case CAMERA_PERSPECTIVE:
            slice_mode = SLICE_ARB;
          break;
          default:
            slice_mode = SLICE_Z;
        }
        ((mafPipeVolumeSlice *)pipe)->InitializeSliceParameters(slice_mode,false);
        ((mafPipeVolumeSlice *)pipe)->ShowUnit(slice_mode != SLICE_ORTHO,m_Rwi->m_Camera);
      }
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
      if (m_NumberOfVisibleVme == 1)
      {
        mafEventMacro(mafEvent(this,CAMERA_RESET));
      }
      else
      {
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void mafViewSlice::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 0)
    m_NumberOfVisibleVme = 0;
  else
    m_NumberOfVisibleVme--;
  if (vme->IsMAFType(mafVMEVolume))
  {
    m_CurrentVolume = NULL;
    if (m_AttachCamera)
    {
      m_AttachCamera->SetVme(NULL);
    }
  }
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//-------------------------------------------------------------------------
int mafViewSlice::GetNodeStatus(mafNode *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  if (m_Sg != NULL)
  {
    if (vme->IsMAFType(mafVMEVolume))
    {
      n = m_Sg->Vme2Node(vme);
      n->m_Mutex = true;
    }
    else if (vme->IsMAFType(mafVMESlicer))
    {
      n = m_Sg->Vme2Node(vme);
      n->m_PipeCreatable = false;
    }
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mmgGui *mafViewSlice::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      default:
        mafEventMacro(*maf_event);
      break;
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafViewSlice::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
  mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeSlice"))
  {
    mafPipeVolumeSlice *pipe = (mafPipeVolumeSlice *)m_CurrentVolume->m_Pipe;
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void mafViewSlice::SetSlice(double origin[3])
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
  mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeSlice"))
  {
    mafPipeVolumeSlice *pipe = (mafPipeVolumeSlice *)m_CurrentVolume->m_Pipe;
    pipe->SetSlice(origin); 
  }
}
