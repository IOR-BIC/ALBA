/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewRX
 Authors: Paolo Quadrani , Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = false;

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaViewRX.h"
#include "albaPipeFactory.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaAttachCamera.h"
#include "albaGUI.h"
#include "albaPipe.h"
#include "albaPipeVolumeProjected.h"
#include "albaVME.h"
#include "albaVMEVolume.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMESlicer.h"
#include "albaPipeSurfaceSlice.h"
#include "albaVisualPipeSlicerSlice.h"
#include "albaVMEVolumeGray.h"
#include "albaAbsMatrixPipe.h"
#include "albaTransform.h"

#include "albaPipeVolumeProjected.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkDataSet.h"
#include "vtkALBARayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewRX);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewRX::albaViewRX(wxString label, int camera_position, bool show_axes, bool show_grid,  int stereo)
:albaViewVTK(label,camera_position,show_axes,show_grid,stereo)
{
  m_CurrentVolume = NULL;
	m_CurrentSide = ALL;
}
//----------------------------------------------------------------------------
albaViewRX::~albaViewRX()
{
}
//----------------------------------------------------------------------------
albaView *albaViewRX::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewRX *v = new albaViewRX(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaViewRX::Create()
{
  if(m_LightCopyEnabled) return; //COPY_LIGHT

  m_Rwi = new albaRWI(albaGetFrame(), TWO_LAYER, m_ShowGrid, m_ShowAxes, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new albaSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack,m_Rwi->m_AlwaysVisibleRenderer);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();
}
//----------------------------------------------------------------------------
void albaViewRX::VmeCreatePipe(albaVME *vme)
{
  albaString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  albaSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->GetPipe());

  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;
    albaPipeFactory *pipe_factory  = albaPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    albaObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    albaPipe *pipe = (albaPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      if (pipe_name.Equals("albaPipeVolumeProjected"))
      {
        ((albaPipeVolumeProjected *)pipe)->InitializeProjectParameters(m_CameraPositionId);
        m_CurrentVolume = n;
        if (m_AttachCamera)
        {
          m_AttachCamera->SetVme(m_CurrentVolume->GetVme());
          CameraUpdate();
        }
				ShowSideVolume(m_CurrentSide);
      }
      else if(pipe_name.Equals("albaPipeSurfaceSlice"))
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
        ((albaPipeSurfaceSlice *)pipe)->SetSlice(positionSlice, normal);
      }
      else if(pipe_name.Equals("albaVisualPipeSlicerSlice"))
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

        //albaSmartPointer<albaVMEVolumeGray> volume;
        albaVME *volume;

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

        ((albaVisualPipeSlicerSlice *)pipe)->SetSlice1(positionSlice1);
        ((albaVisualPipeSlicerSlice *)pipe)->SetSlice2(positionSlice2);
        ((albaVisualPipeSlicerSlice *)pipe)->SetNormal(normal);
        

      }
      pipe->Create(n);
    }
    else
      albaErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void albaViewRX::VmeDeletePipe(albaVME *vme)
{
  albaSceneNode *n = m_Sg->Vme2Node(vme);
  
	m_NumberOfVisibleVme--;

	if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
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
int albaViewRX::GetNodeStatus(albaVME *vme)
{
  albaSceneNode *n = NULL;
	
	albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
	if (lm)
	{
		albaVMELandmarkCloud *lmc = albaVMELandmarkCloud::SafeDownCast(lm->GetParent());
		if (lmc)
		{
			if ((m_Sg->GetNodeStatus(lmc) == NODE_VISIBLE_ON) && lmc->IsLandmarkShow(lm))
				return NODE_VISIBLE_ON;
		}
	}
	
  if (m_Sg != NULL)
  {
    if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
    {
      n = m_Sg->Vme2Node(vme);
      if (n != NULL)
      {
        n->SetMutex(true);
      }
    }
    else if (vme->IsALBAType(albaVMESlicer))
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
albaGUI *albaViewRX::CreateGui()
{
  assert(m_Gui == NULL);
  m_Gui = albaView::CreateGui();

  m_AttachCamera = new albaAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());
	m_Gui->Divider();
  return m_Gui;
}

/*//----------------------------------------------------------------------------
void albaViewRX::VmeShow(albaVME *vme, bool show)
//----------------------------------------------------------------------------
{
  albaViewVTK::VmeShow(vme,show);
  albaSceneNode *SN = this->GetSceneGraph()->Vme2Node(vme);
  
  albaVisualPipeSlicerSlice *pipeSlicer = albaVisualPipeSlicerSlice::SafeDownCast(SN->m_Pipe);
  if(pipeSlicer)
  {
    pipeSlicer->SetThickness(3);
  }
  albaPipeSurfaceSlice *pipe = albaPipeSurfaceSlice ::SafeDownCast(SN->m_Pipe);
  if(pipe)
  {
    pipe->SetThickness(3);
  }
  CameraUpdate();
}*/

//----------------------------------------------------------------------------
void albaViewRX::OnEvent(albaEventBase *alba_event)
{
  Superclass::OnEvent(alba_event);
}

//----------------------------------------------------------------------------
void albaViewRX::SetLutRange(double low_val, double high_val)
{
  if(!m_CurrentVolume) 
    return;
  albaString pipe_name = m_CurrentVolume->GetPipe()->GetTypeName();
  if (pipe_name.Equals("albaPipeVolumeProjected"))
  {
    albaPipeVolumeProjected *pipe = (albaPipeVolumeProjected *)m_CurrentVolume->GetPipe();
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void albaViewRX::GetLutRange(double minMax[2])
{
  if(!m_CurrentVolume) 
    return;
	albaPipe * pipe = m_CurrentVolume->GetPipe();
  albaString pipe_name = pipe->GetTypeName();
  if (pipe_name.Equals("albaPipeVolumeProjected"))
  {
    albaPipeVolumeProjected *volpipe = (albaPipeVolumeProjected *)pipe;
    volpipe->GetLutRange(minMax); 
  }
}

//----------------------------------------------------------------------------
void albaViewRX::CameraUpdate()
{
  if (m_CurrentVolume)
  {
    albaVME *volume = m_CurrentVolume->GetVme();

    std::ostringstream stringStream;
    stringStream << "VME " << volume->GetName() << " ABS matrix:" << std::endl;

    volume->GetAbsMatrixPipe()->GetMatrixPointer()->Print(stringStream);

    m_NewABSPose = volume->GetAbsMatrixPipe()->GetMatrix();

    if (DEBUG_MODE == true)
      albaLogMessage(stringStream.str().c_str());

    if (m_NewABSPose.Equals(&m_OldABSPose))
    { 
      if (DEBUG_MODE == true)
        albaLogMessage("Calling Superclass Camera Update ");

      Superclass::CameraUpdate();
    }
    else
    {
      if (DEBUG_MODE == true)
        albaLogMessage("Calling Rotated Volumes Camera Update ");
      m_OldABSPose = m_NewABSPose;
      CameraUpdateForRotatedVolumes();
    }
  }
  else
  {
    if (DEBUG_MODE == true)
      albaLogMessage("Calling Superclass Camera Update ");
    
    Superclass::CameraUpdate();
  
  }
}

//----------------------------------------------------------------------------
void albaViewRX::SetCameraParallelToDataSetLocalAxis( int axis )
{
  double oldCameraPosition[3] = {0,0,0};
  double oldCameraFocalPoint[3] = {0,0,0};
  double *oldCameraOrientation;
	
  this->GetRWI()->GetCamera()->GetFocalPoint(oldCameraFocalPoint);
  this->GetRWI()->GetCamera()->GetPosition(oldCameraPosition);
  oldCameraOrientation = this->GetRWI()->GetCamera()->GetOrientation();

  albaVME *currentVMEVolume = m_CurrentVolume->GetVme();
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

  if (axis  == albaTransform::X)
  {
    albaTransform::GetVersor(albaTransform::Z,albaMatrix(vmeABSMatrix),newCameraViewUp );

    double xVersor[3] = {0,0,0};

    albaTransform::GetVersor(albaTransform::X,albaMatrix(vmeABSMatrix),xVersor );
    albaTransform::MultiplyVectorByScalar(100, xVersor, xVersor);
    albaTransform::AddVectors(newCameraFocalPoint, xVersor, newCameraPosition);
  }
  else if (axis == albaTransform::Y)
  {
    albaTransform::GetVersor(albaTransform::Z,albaMatrix(vmeABSMatrix),newCameraViewUp );

    double yVersor[3] = {0,0,0};  

    albaTransform::GetVersor(albaTransform::Y,albaMatrix(vmeABSMatrix),yVersor );
    albaTransform::MultiplyVectorByScalar(-100, yVersor, yVersor);
    albaTransform::AddVectors(newCameraFocalPoint, yVersor, newCameraPosition);
  }
  else if (axis == albaTransform::Z)
  {
    albaTransform::GetVersor(albaTransform::Y,albaMatrix(vmeABSMatrix),newCameraViewUp );
    albaTransform::MultiplyVectorByScalar(-1, newCameraViewUp, newCameraViewUp);

    double zVersor[3] = {0,0,0};

    albaTransform::GetVersor(albaTransform::Z,albaMatrix(vmeABSMatrix),zVersor );
    albaTransform::MultiplyVectorByScalar(-100, zVersor, zVersor);
    albaTransform::AddVectors(newCameraFocalPoint, zVersor, newCameraPosition);
  }

  vtkCamera *camera = this->GetRWI()->GetCamera();
  camera->SetFocalPoint(newCameraFocalPoint);
  camera->SetPosition(newCameraPosition);
  camera->SetViewUp(newCameraViewUp);
  camera->SetClippingRange(0.1,1000);
}

//----------------------------------------------------------------------------
void albaViewRX::CameraUpdateForRotatedVolumes()
{
  if (m_CurrentVolume != NULL)
  {
    if (m_CameraPositionId == CAMERA_RX_FRONT)
    {
      SetCameraParallelToDataSetLocalAxis(albaTransform::Y);
    } 
    else if (m_CameraPositionId == CAMERA_RX_LEFT)
    {
      SetCameraParallelToDataSetLocalAxis(albaTransform::X);
    }
  }

  Superclass::CameraUpdate();
}

//----------------------------------------------------------------------------
void albaViewRX::ShowSideVolume(VOLUME_SIDE side)
{
	m_CurrentSide = side;
	if (m_CurrentVolume != NULL)
	{
		int dims[3], range[2];

		vtkDataSet *vtk_data = m_CurrentVolume->GetVme()->GetOutput()->GetVTKData();

		if (NULL != vtk_data)
		{
			if (vtk_data->IsA("vtkRectilinearGrid")) {
				vtkRectilinearGrid *rectilinearGrid = vtkRectilinearGrid::SafeDownCast(vtk_data);
				rectilinearGrid->GetDimensions(dims);
			}
			else if (vtk_data->IsA("vtkImageData")) {
				vtkImageData *imageData = vtkImageData::SafeDownCast(vtk_data);
				imageData->GetDimensions(dims);
			}
		}

		range[0] = 0;
		range[1] = dims[0];

		albaPipeVolumeProjected *pipeVolumeProjected = albaPipeVolumeProjected::SafeDownCast(m_CurrentVolume->GetPipe());

		if (pipeVolumeProjected)
		{
			if (side == ALL)
			{
				pipeVolumeProjected->EnableRangeProjection(false);
			}
			else
			{
				if (side == SIDE_RIGHT)
					range[1] = dims[0] / 2;

				if (side == SIDE_LEFT)
					range[0] = dims[0] / 2;

				pipeVolumeProjected->SetProjectionRange(range);
				pipeVolumeProjected->EnableRangeProjection(true);
			}
		}
	}
}