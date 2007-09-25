/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRX.cpp,v $
  Language:  C++
  Date:      $Date: 2007-09-25 16:06:40 $
  Version:   $Revision: 1.13 $
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

#include "mafViewRX.h"
#include "mafPipeFactory.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"
#include "mmgGui.h"
#include "mafPipe.h"
#include "mafPipeVolumeProjected.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESlicer.h"
#include "mafPipeSurfaceSlice.h"
#include "medVisualPipeSlicerSlice.h"
#include "mafVMEVolumeGray.h"

#include "vtkDataSet.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewRX);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewRX::mafViewRX(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid,show_ruler,stereo)
//----------------------------------------------------------------------------
{
  m_CurrentVolume = NULL;
}
//----------------------------------------------------------------------------
mafViewRX::~mafViewRX()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafView *mafViewRX::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewRX *v = new mafViewRX(m_Label, m_CameraPosition, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewRX::Create()
//----------------------------------------------------------------------------
{
  m_Rwi = new mafRWI(mafGetFrame(), TWO_LAYER, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();
}
//----------------------------------------------------------------------------
void mafViewRX::VmeCreatePipe(mafNode *vme)
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
    if (pipe)
    {
      pipe->SetListener(this);
      if (pipe_name.Equals("mafPipeVolumeProjected"))
      {
        ((mafPipeVolumeProjected *)pipe)->InitializeProjectParameters(m_CameraPosition);
        m_CurrentVolume = n;
        if (m_AttachCamera)
        {
          m_AttachCamera->SetVme(m_CurrentVolume->m_Vme);
          CameraUpdate();
        }
      }
      else if(pipe_name.Equals("mafPipeSurfaceSlice"))
      {
        double normal[3];
        switch(m_CameraPosition)
        {
        case CAMERA_RX_FRONT:
          normal[0] = 0;
          normal[1] = 1;
          normal[2] = 0;
          break;
        case CAMERA_RX_LEFT:
          normal[0] = 1;
          normal[1] = 0;
          normal[2] = 0;
          break;
        case CAMERA_OS_P:
          break;
          //case CAMERA_OS_REP:
          //	this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
        case CAMERA_PERSPECTIVE:
          break;
        default:
          normal[0] = 0;
          normal[1] = 0;
          normal[2] = 1;
        }
        
        double positionSlice[3];
        double b[6];

        mafVMEVolumeGray::SafeDownCast(m_CurrentVolume->m_Vme)->GetOutput()->GetBounds(b);
        positionSlice[0] = (b[1]+b[0])/2;
        positionSlice[1] = (b[3]+b[2])/2;
        positionSlice[2] = (b[5]+b[4])/2;
        ((mafPipeSurfaceSlice *)pipe)->SetSlice(positionSlice);
        ((mafPipeSurfaceSlice *)pipe)->SetNormal(normal);

      }
      else if(pipe_name.Equals("medVisualPipeSlicerSlice"))
      {
        double normal[3];
        switch(m_CameraPosition)
        {
        case CAMERA_RX_FRONT:
          normal[0] = 0;
          normal[1] = 1;
          normal[2] = 0;
          break;
        case CAMERA_RX_LEFT:
          normal[0] = 1;
          normal[1] = 0;
          normal[2] = 0;
          break;
        case CAMERA_OS_P:
          break;
          //case CAMERA_OS_REP:
          //	this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
        case CAMERA_PERSPECTIVE:
          break;
        default:
          normal[0] = 0;
          normal[1] = 0;
          normal[2] = 1;
        }

        double positionSlice1[3],positionSlice2[3];
        double b[6];

        mafSmartPointer<mafVMEVolumeGray> volume;

        volume->DeepCopy(mafVMEVolumeGray::SafeDownCast(m_CurrentVolume->m_Vme));
        volume->GetOutput()->GetBounds(b);
 
        double value1; 
        double value2; 

        if(m_CameraPosition == CAMERA_RX_FRONT)
        {
          value1= b[3] > b[2] ? b[3] : b[2];
          value2= b[3] > b[2] ? b[2] : b[3];
          positionSlice1[0] = (b[1]+b[0])/2;
          positionSlice1[1] = value1 - 0.001;
          positionSlice1[2] = (b[5]+b[4])/2;

          positionSlice2[0] = (b[1]+b[0])/2;
          positionSlice2[1] = value2 + 0.001;
          positionSlice2[2] = (b[5]+b[4])/2;
        }
        else if(m_CameraPosition == CAMERA_RX_LEFT)
        {
          value1= b[1] > b[0] ? b[1] : b[0];
          value2= b[1] > b[0] ? b[0] : b[1];
          positionSlice1[0] = value1 - 0.001;
          positionSlice1[1] = (b[3]+b[2])/2;
          positionSlice1[2] = (b[5]+b[4])/2;
          
          positionSlice2[0] = value2 + 0.001;
          positionSlice2[1] = (b[3]+b[2])/2;
          positionSlice2[2] = (b[5]+b[4])/2;
        }

        ((medVisualPipeSlicerSlice *)pipe)->SetSlice1(positionSlice1);
        ((medVisualPipeSlicerSlice *)pipe)->SetSlice2(positionSlice2);
        ((medVisualPipeSlicerSlice *)pipe)->SetNormal(normal);
        

      }
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void mafViewRX::VmeDeletePipe(mafNode *vme)
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
int mafViewRX::GetNodeStatus(mafNode *vme)
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
      n->m_PipeCreatable = true;
      n->m_Mutex = true;
    }
  }
  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mmgGui *mafViewRX::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());
	m_Gui->Divider();
  return m_Gui;
}
/*//----------------------------------------------------------------------------
void mafViewRX::VmeShow(mafNode *vme, bool show)
//----------------------------------------------------------------------------
{
  mafViewVTK::VmeShow(vme,show);
  mafSceneNode *SN = this->GetSceneGraph()->Vme2Node(vme);
  
  medVisualPipeSlicerSlice *pipeSlicer = medVisualPipeSlicerSlice::SafeDownCast(SN->m_Pipe);
  if(pipeSlicer)
  {
    pipeSlicer->SetThickness(3);
  }
  mafPipeSurfaceSlice *pipe = mafPipeSurfaceSlice ::SafeDownCast(SN->m_Pipe);
  if(pipe)
  {
    pipe->SetThickness(3);
  }
  CameraUpdate();
}*/
//----------------------------------------------------------------------------
void mafViewRX::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*maf_event);
}
//----------------------------------------------------------------------------
void mafViewRX::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
  mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeProjected"))
  {
    mafPipeVolumeProjected *pipe = (mafPipeVolumeProjected *)m_CurrentVolume->m_Pipe;
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void mafViewRX::GetLutRange(double minMax[2])
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
  mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeProjected"))
  {
    mafPipeVolumeProjected *pipe = (mafPipeVolumeProjected *)m_CurrentVolume->m_Pipe;
    pipe->GetLutRange(minMax); 
  }
}
