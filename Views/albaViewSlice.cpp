/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewSlice
 Authors: Gianluigi Crimi, Paolo Quadrani , Stefano Perticoni , Josef Kohout
 
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

#include "albaGUI.h"
#include "albaIndent.h"
#include "albaViewSlice.h"
#include "albaPipeSurfaceSlice.h"
#include "albaPipePolylineSlice.h"
#include "albaPipeMeshSlice.h"

#include "albaPipeSurfaceSlice.h"
#include "albaPipePolylineSlice.h"

#include "albaVME.h"
#include "albaVMEVolume.h"
#include "albaVMESlicer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaPipeFactory.h"
#include "albaPipe.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaAttachCamera.h"
#include "albaPipePolylineGraphEditor.h"
#include "albaTransform.h"
#include "albaAbsMatrixPipe.h"

#include "vtkDataSet.h"
#include "vtkALBARayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkCoordinate.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkCamera.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaViewSlice);
//----------------------------------------------------------------------------

#include "albaMemDbg.h"
#include "albaPipeVolumeOrthoSlice.h"
#include "albaPipeVolumeArbSlice.h"

const int LAST_SLICE_ORIGIN_VALUE_NOT_INITIALIZED = 0;

//----------------------------------------------------------------------------
albaViewSlice::albaViewSlice(wxString label /* =  */, int camera_position /* = CAMERA_CT */, bool show_axes /* = false */, bool show_grid /* = false */,  int stereo /* = 0 */,bool showTICKs/* =false */,bool textureInterpolate/* =true */)
:albaViewVTK(label,camera_position,show_axes,show_grid, stereo)
{
  m_CurrentVolume = NULL;
  m_Border        = NULL;
  
  m_Slice[0] = m_Slice[1] = m_Slice[2] = 0.0;

  m_TextActor=NULL;
  m_TextMapper=NULL;
  m_TextColor[0]=1;
  m_TextColor[1]=0;
  m_TextColor[2]=0;

  m_ShowVolumeTICKs = showTICKs;
	  
  m_TrilinearInterpolationOn = true;
  m_TextureInterpolate = textureInterpolate;

	SetDecimals(2);

	InitializeSlice(m_Slice);     //to correctly set the normal
  m_SliceInitialized = false;   //reset initialized to false
}
//----------------------------------------------------------------------------
albaViewSlice::~albaViewSlice()
{
  BorderDelete();
  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);
	m_SlicingVector.clear();
}
//----------------------------------------------------------------------------
albaView *albaViewSlice::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
  m_LightCopyEnabled = lightCopyEnabled;
  albaViewSlice *v = new albaViewSlice(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType,m_ShowVolumeTICKs,m_TextureInterpolate);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaViewSlice::Create()
{
  if(m_LightCopyEnabled) return; //COPY_LIGHT

  RWI_LAYERS num_layers = m_CameraPositionId != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;
  
  m_Rwi = new albaRWI(albaGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_StereoType);
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

  // text stuff
  m_Text = "";
  m_TextMapper = vtkTextMapper::New();
  m_TextMapper->SetInput(m_Text.ToAscii());

  m_TextActor = vtkActor2D::New();
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->SetPosition(3,3);
  m_TextActor->GetProperty()->SetColor(m_TextColor);

  m_Rwi->m_RenFront->AddActor(m_TextActor);
}
//----------------------------------------------------------------------------
void albaViewSlice::SetTextColor(double color[3])
{
  m_TextColor[0]=color[0];
  m_TextColor[1]=color[1];
  m_TextColor[2]=color[2];
  m_TextActor->GetProperty()->SetColor(m_TextColor);
  m_TextMapper->Modified();
}
//----------------------------------------------------------------------------
void albaViewSlice::UpdateText(int ID)
{
	if (m_TextMapper)
	{
		m_Text = "";

		if (ID == 1)
		{
			switch (m_CameraPositionId)
			{
				case CAMERA_OS_X:
					m_Text = "X = ";
					m_Text += albaString::Format(m_TextFormat, m_Slice[0]);
					break;
				case CAMERA_OS_Y:
					m_Text = "Y = ";
					m_Text += albaString::Format(m_TextFormat, m_Slice[1]);
					break;
				case CAMERA_OS_Z:
					m_Text = "Z = ";
					m_Text += albaString::Format(m_TextFormat, m_Slice[2]);
					break;
				default:
					break;
			}
			m_TextMapper->SetInput(m_Text.ToAscii());
			m_TextMapper->Modified();
		}
		else
		{
			m_Text = "";
			m_TextMapper->SetInput(m_Text.ToAscii());
			m_TextMapper->Modified();
		}
	}
}
//----------------------------------------------------------------------------
void albaViewSlice::InitializeSlice(double* Origin)
{
	memcpy(m_Slice, Origin, sizeof(m_Slice));

	if (m_CameraPositionId == CAMERA_ARB)
		this->GetRWI()->GetCamera()->GetViewPlaneNormal(m_SliceNormal);
	else
	{
		m_SliceNormal[0] = m_SliceNormal[1] = m_SliceNormal[2] = 0.0;

		switch (m_CameraPositionId)
		{
			case CAMERA_OS_X:
				m_SliceNormal[0] = 1;
				break;
			case CAMERA_OS_Y:
				m_SliceNormal[1] = 1;
				break;
				//case CAMERA_OS_Z, CAMERA_OS_P
			default:
				m_SliceNormal[2] = 1;
				break;
		}
	}
	
	m_SliceInitialized = true;
}

//----------------------------------------------------------------------------
void albaViewSlice::SetDecimals(int decimals)
{
	m_Decimals = decimals; 
	m_TextFormat.Printf("%%.%df",decimals);
	UpdateText();
	if(m_Rwi)
		CameraUpdate();
}

//----------------------------------------------------------------------------
void albaViewSlice::VmeCreatePipe(albaVME *vme)
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
    if (pipe != NULL)
    {
      pipe->SetListener(this);
      if (pipe->IsA("albaPipeVolumeOrthoSlice"))  
      {
        m_CurrentVolume = n;

        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->GetVme());
        int slice_mode;
        vtkDataSet *data = vme->GetOutput()->GetVTKData();
        assert(data);
        switch(m_CameraPositionId)
        {
        case CAMERA_OS_X:
          slice_mode = albaPipeVolumeOrthoSlice::SLICE_X;
          break;
        case CAMERA_OS_Y:
          slice_mode = albaPipeVolumeOrthoSlice::SLICE_Y;
          break;
        case CAMERA_OS_P:
          slice_mode = albaPipeVolumeOrthoSlice::SLICE_ORTHO;
          break;
        default:
          slice_mode = albaPipeVolumeOrthoSlice::SLICE_Z;
        }
        if (m_SliceInitialized)
        {
          ((albaPipeVolumeOrthoSlice *)pipe)->InitializeSliceParameters(slice_mode, m_Slice, false,false,m_TextureInterpolate);
          ((albaPipeVolumeOrthoSlice *)pipe)->SetNormal(m_SliceNormal);
        }
        else
        {
          ((albaPipeVolumeOrthoSlice *)pipe)->InitializeSliceParameters(slice_mode,false,false,m_TextureInterpolate);
        }

        if(m_ShowVolumeTICKs)
          ((albaPipeVolumeOrthoSlice *)pipe)->ShowTICKsOn();
        else
          ((albaPipeVolumeOrthoSlice *)pipe)->ShowTICKsOff();
        ((albaPipeVolumeOrthoSlice *)pipe)->SetInterpolation(m_TrilinearInterpolationOn);
				UpdateText();
      }
			else if (pipe->IsA("albaPipeVolumeArbSlice"))
			{
				m_CurrentVolume = n;

				if (m_AttachCamera)
					m_AttachCamera->SetVme(m_CurrentVolume->GetVme());
				int slice_mode;
				vtkDataSet *data = vme->GetOutput()->GetVTKData();
				assert(data);
				if (m_SliceInitialized)
				{
					((albaPipeVolumeArbSlice *)pipe)->InitializeSliceParameters(m_Slice, false, false, m_TextureInterpolate);
					((albaPipeVolumeArbSlice *)pipe)->SetNormal(m_SliceNormal);
				}
				else
				{
					((albaPipeVolumeArbSlice *)pipe)->InitializeSliceParameters(false, false, m_TextureInterpolate);
				}

				if (m_ShowVolumeTICKs)
					((albaPipeVolumeArbSlice *)pipe)->ShowTICKsOn();
				else
					((albaPipeVolumeArbSlice *)pipe)->ShowTICKsOff();
				((albaPipeVolumeArbSlice *)pipe)->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
				UpdateText();
			}
      else 
      {
        //not a VolumeSlice pipe, check, if it is some slicer
        albaPipeSlice* spipe = albaPipeSlice::SafeDownCast(pipe);
        if (spipe != NULL)
        { 
					albaPipeMeshSlice *meshPipe;
					if(pipe->IsA("albaPipePolylineGraphEditor"))
          {
            if(m_CameraPositionId==CAMERA_OS_P)
              ((albaPipePolylineGraphEditor *)pipe)->SetModalityPerspective();
            else
              ((albaPipePolylineGraphEditor *)pipe)->SetModalitySlice();				
          }
					else if (meshPipe = albaPipeMeshSlice::SafeDownCast(pipe))
					{
						if  (m_CameraPositionId == CAMERA_OS_X)
							meshPipe->SetFlipNormalOff();
					}
          //common stuff
					m_SlicingVector.push_back(n);
          double positionSlice[3];
          positionSlice[0] = m_Slice[0];
          positionSlice[1] = m_Slice[1];
          positionSlice[2] = m_Slice[2];
          MultiplyPointByInputVolumeABSMatrix(positionSlice);
          spipe->SetSlice(positionSlice, m_SliceNormal);
        }
      } //end else [it is not volume slicing]                     

      pipe->Create(n);
    }
    else
      albaErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void albaViewSlice::VmeDeletePipe(albaVME *vme)
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

  if(vme->IsALBAType(albaVMELandmark))
    RemoveFromSlicingList(vme);
}
//-------------------------------------------------------------------------
int albaViewSlice::GetNodeStatus(albaVME *vme)
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
		n = m_Sg->Vme2Node(vme);
		if (vme->GetOutput()->IsA("albaVMEOutputVolume") || vme->IsALBAType(albaVMESlicer))
		{
			if (n != NULL)
				n->SetMutex(true);
		}
		else if (vme->IsALBAType(albaVMEImage))
		{
			if (n != NULL)
				n->SetPipeCreatable(false);
		}
	}


  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
albaGUI *albaViewSlice::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();

	m_AttachCamera = new albaAttachCamera(m_Gui, m_Rwi, this);
	m_Gui->AddGui(m_AttachCamera->GetGui());

	// Added by Losi 11.25.2009
	if (m_CurrentVolume)
	{
		albaPipeVolumeOrthoSlice *po = NULL;
		po = albaPipeVolumeOrthoSlice::SafeDownCast(this->GetNodePipe(m_CurrentVolume->GetVme()));
		if (po) // Is this required?
			po->SetInterpolation(m_TrilinearInterpolationOn);

		albaPipeVolumeArbSlice *pa = NULL;
		pa = albaPipeVolumeArbSlice::SafeDownCast(this->GetNodePipe(m_CurrentVolume->GetVme()));
		if (pa) // Is this required?
			pa->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
	};
	m_Gui->Divider(1);
	m_Gui->Bool(ID_TRILINEAR_INTERPOLATION, "Interpolation", &m_TrilinearInterpolationOn, 1);

	m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void albaViewSlice::OnEvent(albaEventBase *alba_event)
{
  // Added by Losi 11.25.2009
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(alba_event->GetId()) 
    {
      case ID_TRILINEAR_INTERPOLATION:
        {
          if (m_CurrentVolume)
          {
            albaPipeVolumeOrthoSlice *po = NULL;
            po = albaPipeVolumeOrthoSlice::SafeDownCast(this->GetNodePipe(m_CurrentVolume->GetVme()));
            if(po)
            {
              po->SetInterpolation(m_TrilinearInterpolationOn);
              this->CameraUpdate();
            }
						albaPipeVolumeArbSlice *pa = NULL;
						pa = albaPipeVolumeArbSlice::SafeDownCast(this->GetNodePipe(m_CurrentVolume->GetVme()));
						if (pa)
						{
							pa->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
							this->CameraUpdate();
						}
          }
        }
        break;
    }
  }

	Superclass::OnEvent(alba_event);
}

//----------------------------------------------------------------------------
void albaViewSlice::SetLutRange(double low_val, double high_val)
{
  if(!m_CurrentVolume) 
    return;
  
  albaPipeVolumeOrthoSlice *pipeOrtho = albaPipeVolumeOrthoSlice::SafeDownCast(m_CurrentVolume->GetPipe());
  if (pipeOrtho != NULL) {
		pipeOrtho->SetLutRange(low_val, high_val);
  }
	albaPipeVolumeArbSlice *pipeArb = albaPipeVolumeArbSlice::SafeDownCast(m_CurrentVolume->GetPipe());
	if (pipeArb != NULL) {
		pipeArb->SetLutRange(low_val, high_val);
	}
}
//----------------------------------------------------------------------------
void albaViewSlice::SetSlice(double* Origin, double* Normal)
{
  
  //set slice origin and normal
  if (Origin != NULL)
		memcpy(m_Slice, Origin, sizeof(double) * 3);
  
  if (Normal != NULL)
    memcpy(m_SliceNormal,Normal, sizeof(double) * 3);
  
  //and now set it for every VME
  if (m_CurrentVolume)
	{
    albaPipeSlice* pipe = albaPipeSlice::SafeDownCast(m_CurrentVolume->GetPipe());
		if (pipe != NULL)
			pipe->SetSlice(Origin, NULL);
	}
 
  double coord[3];
  coord[0]= m_Slice[0];
  coord[1]= m_Slice[1];
  coord[2]= m_Slice[2];
  MultiplyPointByInputVolumeABSMatrix(coord);

  for(int i = 0; i < m_SlicingVector.size(); i++)
  {
		albaPipeSlice *pipe = albaPipeSlice::SafeDownCast(m_SlicingVector.at(i)->GetPipe());
		if(pipe)
			pipe->SetSlice(coord, m_SliceNormal);
  }	
	  
  // update text
  this->UpdateText();
}
//----------------------------------------------------------------------------
void albaViewSlice::SetSliceAxis(int sliceAxis)
{
	switch (sliceAxis)
	{
		case 0:
			m_CameraPositionId = CAMERA_OS_X;
			break;
		case 1:
			m_CameraPositionId = CAMERA_OS_Y;
			break;
		case 2:
			m_CameraPositionId = CAMERA_OS_Z;
			break;
		default:
			m_CameraPositionId = CAMERA_OS_P;
	}
		
	if (m_CurrentVolume)
	{
		albaPipeVolumeOrthoSlice* pipe = albaPipeVolumeOrthoSlice::SafeDownCast(m_CurrentVolume->GetPipe());
		if (pipe != NULL)
			pipe->SetSliceDirection(sliceAxis);
	}
	InitializeSlice(m_Slice);
	if(m_CurrentVolume)
		SetCameraParallelToDataSetLocalAxis(sliceAxis);
}
//----------------------------------------------------------------------------
void albaViewSlice::GetSlice(double* Origin, double* Normal)
{
  if (Origin != NULL)
    memcpy(Origin, m_Slice, sizeof(m_Slice));

  if (Normal != NULL)
    memcpy(Normal, m_SliceNormal, sizeof(m_SliceNormal));
}

//----------------------------------------------------------------------------
void albaViewSlice::BorderUpdate()
{
  if (NULL != m_Border)
		BorderCreate(m_BorderColor);
}
//----------------------------------------------------------------------------
void albaViewSlice::BorderCreate(double col[3])
{
  m_BorderColor[0] = col[0];
  m_BorderColor[1] = col[1];
  m_BorderColor[2] = col[2];

  if(m_Border) BorderDelete();
  int size[2];
  this->GetWindow()->GetSize(&size[0],&size[1]);
  vtkPlaneSource *ps = vtkPlaneSource::New();
  ps->SetOrigin(0, 0, 0);
  ps->SetPoint1(size[0]-1, 0, 0);
  ps->SetPoint2(0, size[1]-1, 0);

  vtkOutlineFilter *of = vtkOutlineFilter::New();
  of->SetInputConnection(ps->GetOutputPort());

  vtkCoordinate *coord = vtkCoordinate::New();
  coord->SetCoordinateSystemToDisplay();
  coord->SetValue(size[0]-1, size[1]-1, 0);

  vtkPolyDataMapper2D *pdmd = vtkPolyDataMapper2D::New();
  pdmd->SetInputConnection(of->GetOutputPort());
  pdmd->SetTransformCoordinate(coord);

  vtkProperty2D *pd = vtkProperty2D::New();
  pd->SetDisplayLocationToForeground();
  pd->SetLineWidth(4);
  pd->SetColor(col[0],col[1],col[2]);

  m_Border = vtkActor2D::New();
  m_Border->SetMapper(pdmd);
  m_Border->SetProperty(pd);
  m_Border->SetPosition(1,1);

  m_Rwi->m_RenFront->AddActor(m_Border);

  vtkDEL(ps);
  vtkDEL(of);
  vtkDEL(coord);
  vtkDEL(pdmd);
  vtkDEL(pd);
}
//----------------------------------------------------------------------------
void albaViewSlice::SetBorderOpacity(double value)
{
  if(m_Border)
  {
    m_Border->GetProperty()->SetOpacity(value);
    m_Border->Modified();
  }
}
//----------------------------------------------------------------------------
void albaViewSlice::BorderDelete()
{
  if(m_Border)
  {
    m_Rwi->m_RenFront->RemoveActor(m_Border);
    vtkDEL(m_Border);
  }  
}

//----------------------------------------------------------------------------
void albaViewSlice::RemoveFromSlicingList(albaVME *vme)
{
  albaSceneNode *sceneNode = m_Sg->Vme2Node(vme);
  for(int i=0;i<m_SlicingVector.size();i++)
    if (sceneNode== m_SlicingVector[i])
			m_SlicingVector.erase(m_SlicingVector.begin()+i);
}
//----------------------------------------------------------------------------
void albaViewSlice::VmeShow(albaVME *vme, bool show)
{
  if (vme->GetOutput()->IsA("albaVMEOutputVolume"))
  {
    if (show)
    {
			if(m_AttachCamera)
				m_AttachCamera->SetVme(vme);
	  }
    else
    {
			if(m_AttachCamera)
				m_AttachCamera->SetVme(NULL);
      this->UpdateText(0);
    }
  }
	else if (show == false)
    this->RemoveFromSlicingList(vme);

	Superclass::VmeShow(vme, show);
}
//----------------------------------------------------------------------------
void albaViewSlice::VmeRemove(albaVME *vme)
{
  this->RemoveFromSlicingList(vme);
  Superclass::VmeRemove(vme);
}
//-------------------------------------------------------------------------
void albaViewSlice::Print(std::ostream& os, const int tabs)// const
{
  albaIndent indent(tabs);

  os << indent << "albaViewSlice" << '\t' << this << std::endl;
  os << indent << "Name" << '\t' << m_Label << std::endl;
  os << std::endl;
  m_Sg->Print(os,1);
}
//-------------------------------------------------------------------------
void albaViewSlice::MultiplyPointByInputVolumeABSMatrix(double *point)
{
  if(m_CurrentVolume && m_CurrentVolume->GetVme())
  {
    albaMatrix *mat = m_CurrentVolume->GetVme()->GetAbsMatrixPipe()->GetMatrixPointer();
    double coord[4];
    coord[0] = point[0];
    coord[1] = point[1];
    coord[2] = point[2];
    double result[4];

    vtkTransform *newT = vtkTransform::New();
    newT->SetMatrix(mat->GetVTKMatrix());
    newT->TransformPoint(coord, result);
    vtkDEL(newT);

    point[0] = result[0];
    point[1] = result[1];
    point[2] = result[2];
  } 
}
//-------------------------------------------------------------------------
void albaViewSlice::CameraUpdate()
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
    
    // Fix bug #2085: Added by Losi 05.11.2010
    // Avoid pan & zoom reset while changing timestamp
    albaMatrix oldABSPoseForEquals;
    oldABSPoseForEquals.DeepCopy(&m_OldABSPose);
    oldABSPoseForEquals.SetTimeStamp(m_NewABSPose.GetTimeStamp());

    if (m_NewABSPose.Equals(&oldABSPoseForEquals))
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
//-------------------------------------------------------------------------
void albaViewSlice::SetCameraParallelToDataSetLocalAxis( int axis )
{
  double oldCameraPosition[3] = {0,0,0};
  double oldCameraFocalPoint[3] = {0,0,0};
  double *oldCameraOrientation;

  this->GetRWI()->GetCamera()->GetFocalPoint(oldCameraFocalPoint);
  this->GetRWI()->GetCamera()->GetPosition(oldCameraPosition);
  oldCameraOrientation = this->GetRWI()->GetCamera()->GetOrientation();

  albaVME *currentVMEVolume = m_CurrentVolume->GetVme();
  assert(currentVMEVolume);

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
void albaViewSlice::GetVisualPipeName(albaVME *node, albaString &pipe_name)
{
	assert(node);

	node->Modified();
	vtkDataSet *data = node->GetOutput()->GetVTKData();
	albaVMELandmarkCloud *lmc = albaVMELandmarkCloud::SafeDownCast(node);
	albaVMELandmark *lm = albaVMELandmark::SafeDownCast(node);
	if (lmc == NULL && data == NULL && lm == NULL)
	{
		pipe_name = "albaPipeBox";
	}
	else
	{
		// custom visualization for the view should be considered only
		// if we are not in editing mode.
		albaString vme_type = node->GetTypeName();
		if (m_PipeMap.count(vme_type))
		{
			// pick up the visual pipe from the view's visual pipe map
			pipe_name = m_PipeMap[vme_type].m_PipeName;
		}
	}

	if (pipe_name.IsEmpty())
	{
		// pick up the default visual pipe from the vme
		pipe_name = node->GetVisualPipeSlice();
		if (pipe_name.IsEmpty())
			pipe_name = node->GetVisualPipe();
	}
}

//-------------------------------------------------------------------------
void albaViewSlice::CameraUpdateForRotatedVolumes()
{
	int axis;
	if (m_CameraPositionId == CAMERA_OS_X)
		axis = albaTransform::X;
	else if (m_CameraPositionId == CAMERA_OS_Y)
		axis = albaTransform::Y;
	else //CAMERA_OS_Z, CAMERA_OS_P
		axis = albaTransform::Z;

  if (m_CurrentVolume != NULL)
  {
    SetCameraParallelToDataSetLocalAxis(axis); 
      
    // needed to update surface slices during camera rotation
    if (m_SlicingVector.size()  != 0)
      SetSlice();    

    this->CameraReset(m_CurrentVolume->GetVme());
  }    
  
  Superclass::CameraUpdate();
}
