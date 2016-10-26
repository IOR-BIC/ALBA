/*=========================================================================

 Program: MAF2
 Module: mafViewRX
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = false;

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
#include "mafGUI.h"
#include "mafPipe.h"
#include "mafPipeVolumeProjected.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESlicer.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafVisualPipeSlicerSlice.h"
#include "mafVMEVolumeGray.h"
#include "mafAbsMatrixPipe.h"
#include "mafTransform.h"

#include "vtkDataSet.h"
#include "vtkMAFRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewRX);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewRX::mafViewRX(wxString label, int camera_position, bool show_axes, bool show_grid,  int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid,stereo)
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
mafView *mafViewRX::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewRX *v = new mafViewRX(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewRX::Create()
//----------------------------------------------------------------------------
{
  if(m_LightCopyEnabled) return; //COPY_LIGHT

  m_Rwi = new mafRWI(mafGetFrame(), TWO_LAYER, m_ShowGrid, m_ShowAxes, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack,m_Rwi->m_AlwaysVisibleRenderer);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();
}
//----------------------------------------------------------------------------
void mafViewRX::VmeCreatePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->GetPipe());

  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;
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
        ((mafPipeVolumeProjected *)pipe)->InitializeProjectParameters(m_CameraPositionId);
        m_CurrentVolume = n;
        if (m_AttachCamera)
        {
          m_AttachCamera->SetVme(m_CurrentVolume->GetVme());
          CameraUpdate();
        }
      }
      else if(pipe_name.Equals("mafPipeSurfaceSlice_BES"))
      {
        double normal[3];
        switch(m_CameraPositionId)
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

        m_CurrentVolume->GetVme()->GetOutput()->GetBounds(b);
        positionSlice[0] = (b[1]+b[0])/2;
        positionSlice[1] = (b[3]+b[2])/2;
        positionSlice[2] = (b[5]+b[4])/2;
        ((mafPipeSurfaceSlice_BES *)pipe)->SetSlice(positionSlice, normal);
      }
      else if(pipe_name.Equals("mafVisualPipeSlicerSlice"))
      {
        double normal[3];
        switch(m_CameraPositionId)
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

        //mafSmartPointer<mafVMEVolumeGray> volume;
        mafVME *volume;

        volume =m_CurrentVolume->GetVme();
        volume->GetOutput()->GetBounds(b);
 
        double value1; 
        double value2; 

        if(m_CameraPositionId == CAMERA_RX_FRONT)
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
        else if(m_CameraPositionId == CAMERA_RX_LEFT)
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

        ((mafVisualPipeSlicerSlice *)pipe)->SetSlice1(positionSlice1);
        ((mafVisualPipeSlicerSlice *)pipe)->SetSlice2(positionSlice2);
        ((mafVisualPipeSlicerSlice *)pipe)->SetNormal(normal);
        

      }
      pipe->Create(n);
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void mafViewRX::VmeDeletePipe(mafVME *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  
	m_NumberOfVisibleVme--;

	if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    m_CurrentVolume = NULL;
    if (m_AttachCamera)
    {
      m_AttachCamera->SetVme(NULL);
    }
  }
  assert(n && n->GetPipe());
	n->DeletePipe();
}
//-------------------------------------------------------------------------
int mafViewRX::GetNodeStatus(mafVME *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
	
	mafVMELandmark *lm = mafVMELandmark::SafeDownCast(vme);
	if (lm)
	{
		mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(lm->GetParent());
		if (lmc)
		{
			if ((m_Sg->GetNodeStatus(lmc) == NODE_VISIBLE_ON) && lmc->IsLandmarkShow(lm))
				return NODE_VISIBLE_ON;
		}
	}
	
  if (m_Sg != NULL)
  {
    if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
    {
      n = m_Sg->Vme2Node(vme);
      if (n != NULL)
      {
        n->SetMutex(true);
      }
    }
    else if (vme->IsMAFType(mafVMESlicer))
    {
      n = m_Sg->Vme2Node(vme);
      if (n != NULL)
      {
        n->SetPipeCreatable(true);
        n->SetMutex(true);
      }
    }
  }
  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mafGUI *mafViewRX::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());
	m_Gui->Divider();
  return m_Gui;
}
/*//----------------------------------------------------------------------------
void mafViewRX::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
  mafViewVTK::VmeShow(vme,show);
  mafSceneNode *SN = this->GetSceneGraph()->Vme2Node(vme);
  
  mafVisualPipeSlicerSlice *pipeSlicer = mafVisualPipeSlicerSlice::SafeDownCast(SN->m_Pipe);
  if(pipeSlicer)
  {
    pipeSlicer->SetThickness(3);
  }
  mafPipeSurfaceSlice_BES *pipe = mafPipeSurfaceSlice_BES ::SafeDownCast(SN->m_Pipe);
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
  Superclass::OnEvent(maf_event);
}
//----------------------------------------------------------------------------
void mafViewRX::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
  mafString pipe_name = m_CurrentVolume->GetPipe()->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeProjected"))
  {
    mafPipeVolumeProjected *pipe = (mafPipeVolumeProjected *)m_CurrentVolume->GetPipe();
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void mafViewRX::GetLutRange(double minMax[2])
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
	mafPipe * pipe = m_CurrentVolume->GetPipe();
  mafString pipe_name = pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeProjected"))
  {
    mafPipeVolumeProjected *volpipe = (mafPipeVolumeProjected *)pipe;
    volpipe->GetLutRange(minMax); 
  }
}


void mafViewRX::CameraUpdate()
{
  if (m_CurrentVolume)
  {
    mafVME *volume = m_CurrentVolume->GetVme();

    std::ostringstream stringStream;
    stringStream << "VME " << volume->GetName() << " ABS matrix:" << std::endl;

    volume->GetAbsMatrixPipe()->GetMatrixPointer()->Print(stringStream);

    m_NewABSPose = volume->GetAbsMatrixPipe()->GetMatrix();

    if (DEBUG_MODE == true)
      mafLogMessage(stringStream.str().c_str());

    if (m_NewABSPose.Equals(&m_OldABSPose))
    { 
      if (DEBUG_MODE == true)
        mafLogMessage("Calling Superclass Camera Update ");

      Superclass::CameraUpdate();
    }
    else
    {
      if (DEBUG_MODE == true)
        mafLogMessage("Calling Rotated Volumes Camera Update ");
      m_OldABSPose = m_NewABSPose;
      CameraUpdateForRotatedVolumes();
    }
  }
  else
  {

    if (DEBUG_MODE == true)
      mafLogMessage("Calling Superclass Camera Update ");
    
    Superclass::CameraUpdate();
  
  }
}

void mafViewRX::SetCameraParallelToDataSetLocalAxis( int axis )
{
  double oldCameraPosition[3] = {0,0,0};
  double oldCameraFocalPoint[3] = {0,0,0};
  double *oldCameraOrientation;


  this->GetRWI()->GetCamera()->GetFocalPoint(oldCameraFocalPoint);
  this->GetRWI()->GetCamera()->GetPosition(oldCameraPosition);
  oldCameraOrientation = this->GetRWI()->GetCamera()->GetOrientation();

  mafVME *currentVMEVolume = m_CurrentVolume->GetVme();
  assert(currentVMEVolume);

  assert(m_CurrentVolume);
  vtkDataSet *vmeVTKData = currentVMEVolume->GetOutput()->GetVTKData();
  vtkMatrix4x4 *vmeABSMatrix = currentVMEVolume->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix();

  double absDataBounds[6] = {0,0,0,0,0,0};

  currentVMEVolume->GetOutput()->GetBounds(absDataBounds);

  double newCameraFocalPoint[3] = {0,0,0};

  newCameraFocalPoint[0] = (absDataBounds[0] + absDataBounds[1]) / 2;
  newCameraFocalPoint[1] = (absDataBounds[2] + absDataBounds[3]) / 2;
  newCameraFocalPoint[2] = (absDataBounds[4] + absDataBounds[5]) / 2;

  double newCameraViewUp[3] = {0,0,0};
  double newCameraPosition[3] = {0,0,0};

  if (axis  == mafTransform::X)
  {
    mafTransform::GetVersor(mafTransform::Z,mafMatrix(vmeABSMatrix),newCameraViewUp );

    double xVersor[3] = {0,0,0};

    mafTransform::GetVersor(mafTransform::X,mafMatrix(vmeABSMatrix),xVersor );
    mafTransform::MultiplyVectorByScalar(100, xVersor, xVersor);
    mafTransform::AddVectors(newCameraFocalPoint, xVersor, newCameraPosition);
  }
  else if (axis == mafTransform::Y)
  {
    mafTransform::GetVersor(mafTransform::Z,mafMatrix(vmeABSMatrix),newCameraViewUp );

    double yVersor[3] = {0,0,0};  

    mafTransform::GetVersor(mafTransform::Y,mafMatrix(vmeABSMatrix),yVersor );
    mafTransform::MultiplyVectorByScalar(-100, yVersor, yVersor);
    mafTransform::AddVectors(newCameraFocalPoint, yVersor, newCameraPosition);
  }
  else if (axis == mafTransform::Z)
  {
    mafTransform::GetVersor(mafTransform::Y,mafMatrix(vmeABSMatrix),newCameraViewUp );
    mafTransform::MultiplyVectorByScalar(-1, newCameraViewUp, newCameraViewUp);

    double zVersor[3] = {0,0,0};

    mafTransform::GetVersor(mafTransform::Z,mafMatrix(vmeABSMatrix),zVersor );
    mafTransform::MultiplyVectorByScalar(-100, zVersor, zVersor);
    mafTransform::AddVectors(newCameraFocalPoint, zVersor, newCameraPosition);
  }

  vtkCamera *camera = this->GetRWI()->GetCamera();
  camera->SetFocalPoint(newCameraFocalPoint);
  camera->SetPosition(newCameraPosition);
  camera->SetViewUp(newCameraViewUp);
  camera->SetClippingRange(0.1,1000);

}


void mafViewRX::CameraUpdateForRotatedVolumes()
{
  if (m_CurrentVolume != NULL)
  {
    if (m_CameraPositionId == CAMERA_RX_FRONT)
    {
      SetCameraParallelToDataSetLocalAxis(mafTransform::Y);
    } 
    else if (m_CameraPositionId == CAMERA_RX_LEFT)
    {
      SetCameraParallelToDataSetLocalAxis(mafTransform::X);
    }
  }

  Superclass::CameraUpdate();
}