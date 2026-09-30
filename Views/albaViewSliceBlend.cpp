/*=========================================================================
Program:   Alba
Module:    albaViewSliceBlend.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaIndent.h"
#include "albaGUI.h"
#include "albaPipeSurfaceSlice.h"
#include "albaPipePolylineSlice.h"
#include "albaPipeMeshSlice.h"
#include "albaVME.h"
#include "albaVMEVolume.h"
#include "albaVMESlicer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMEVolumeGray.h"
#include "albaPipeVolumeOrthoSlice.h"
#include "albaPipeFactory.h"
#include "albaPipe.h"
#include "albaRWI.h"
#include "albaSceneGraph.h"
#include "albaAttachCamera.h"
#include "albaGUIFloatSlider.h"
#include "albaGUILutSlider.h"
#include "albaPipePolylineGraphEditor.h"
#include "albaViewSliceBlend.h"
#include "albaPipeVolumeSliceBlend.h"

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
albaCxxTypeMacro(albaViewSliceBlend);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaViewSliceBlend::albaViewSliceBlend(wxString label, int camera_position, bool show_axes, bool show_grid, int stereo)
:albaViewVTK(label,camera_position,show_axes,show_grid, stereo)
{
  // default values
  m_CurrentVolume = NULL;
  m_Border        = NULL;

  //Default values of slices
  m_Slice1[0] = m_Slice1[1] = m_Slice1[2] = 0.0;
  m_Slice2[0] = m_Slice2[1] = m_Slice2[2] = 0.0;

  m_SliceInitialized = false;

  //Default values of opacity is 0.5
  m_Opacity = 0.5;

  m_CurrentSurface.clear();
}
//----------------------------------------------------------------------------
albaViewSliceBlend::~albaViewSliceBlend()
{
  BorderDelete();
  m_CurrentSurface.clear();
}
//----------------------------------------------------------------------------
albaView *albaViewSliceBlend::Copy(albaObserver *Listener, bool lightCopyEnabled /* = false */)
{
  albaViewSliceBlend *v = new albaViewSliceBlend(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void albaViewSliceBlend::Create()  
{
  RWI_LAYERS num_layers = m_CameraPositionId != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;

  m_Rwi = new albaRWI(albaGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new albaSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack, m_Rwi->m_AlwaysVisibleRenderer);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
  m_Picker2D->InitializePickList();
}
//----------------------------------------------------------------------------
void albaViewSliceBlend::InitializeSlice(double slice1[3],double slice2[3])
{
  memcpy(m_Slice1,slice1,sizeof(m_Slice1));
  memcpy(m_Slice2,slice2,sizeof(m_Slice2));
  m_SliceInitialized = true;
}

//----------------------------------------------------------------------------
void albaViewSliceBlend::VmeCreatePipe(albaVME *vme)
{
  int result = GetNodeStatus(vme);

  //If node can't be visualized
  if (result == NODE_NON_VISIBLE)
  {
    return;
  }

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
      // Initialize albaPipeVolumeSliceBlend
      if (pipe_name.Equals("albaPipeVolumeSliceBlend"))
      {
        m_CurrentVolume = n;
        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->GetVme());
        int slice_mode;
        vtkDataSet *data = vme->GetOutput()->GetVTKData();
        assert(data);
        // check the type of camera
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
        //check if slices are initialized
        if (m_SliceInitialized)
        {
          //If slice position is already set
          ((albaPipeVolumeSliceBlend *)pipe)->InitializeSliceParameters(slice_mode,m_Slice1,m_Slice2,false);
        }
        else
        {
          //If slice position isn't already set
          ((albaPipeVolumeSliceBlend *)pipe)->InitializeSliceParameters(slice_mode,false);
        }
        ((albaPipeVolumeSliceBlend *)pipe)->SetSliceOpacity(m_Opacity);
      }
      pipe->Create(n);
    }
    else
      albaErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
}
//----------------------------------------------------------------------------
void albaViewSliceBlend::VmeDeletePipe(albaVME *vme)
{
  albaSceneNode *n = m_Sg->Vme2Node(vme);

	m_NumberOfVisibleVme--;

  //if vme is a volume detach camera
  if (vme->IsALBAType(albaVMEVolume))
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
  {
    //Update list of surfaces visualized
    UpdateSurfacesList(vme);
  }
}

//-------------------------------------------------------------------------
int albaViewSliceBlend::GetNodeStatus(albaVME *vme)
{
  albaSceneNode *n = NULL;
  if (m_Sg != NULL)
  {
    if (vme->IsALBAType(albaVMEVolume))
    {
      n = m_Sg->Vme2Node(vme);
      //Only a volume can be visualized
      n->SetMutex(true);
    }
    else if (vme->IsALBAType(albaVMESlicer))
    {
      n = m_Sg->Vme2Node(vme);
      n->SetPipeCreatable(false);
    }
    else if (vme->IsALBAType(albaVMEImage))
    {
      n = m_Sg->Vme2Node(vme);
      //n->m_Mutex = true;
      //It's impossible visualize vme image
      n->SetPipeCreatable(false);
    }
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}

//-------------------------------------------------------------------------
albaGUI *albaViewSliceBlend::CreateGui()
{
  assert(m_Gui == NULL);
	m_Gui = albaView::CreateGui();
	m_Gui->SetListener(this);

  if (m_CurrentVolume)
  {
    // if a Volume is present use his bounds
    double b[6];
    albaVMEVolumeGray::SafeDownCast(m_CurrentVolume->GetVme())->GetOutput()->GetBounds(b);
    m_Slice1Position = b[4];
    m_Slice2Position = b[5];
  }
  else
  {
    // Otherwise use 0,1 as range
    m_Slice1Position = 0;
    m_Slice2Position = 1;
  }

  // Create attach camera for the volume
  m_AttachCamera = new albaAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->FloatSlider(ID_OPACITY,&m_Opacity,0.0,1.0,_("Cranial"),_("Caudal"));

	//m_Gui->Double(-1, "Opacity", &m_Opacity);
	
	m_Gui->FitGui();

  m_Gui->Enable(ID_OPACITY,m_CurrentVolume!=NULL);

  return m_Gui;
}

//----------------------------------------------------------------------------
void albaViewSliceBlend::OnEvent(albaEventBase *alba_event)  
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
    case ID_OPACITY:
      {
			SetOpacity(m_Opacity);
			m_Gui->Update();
      }
      break;
    default:
      //Other events
			Superclass::OnEvent(alba_event);
      break;
    }
  }
  else
  {
		Superclass::OnEvent(alba_event);
  }
}

//----------------------------------------------------------------------------
void albaViewSliceBlend::SetLutRange(double low_val, double high_val)
{
  if(!m_CurrentVolume) 
    return;
	albaString pipe_name = m_CurrentVolume->GetPipe()->GetTypeName();
  if (pipe_name.Equals("albaPipeVolumeSliceBlend"))
  {
		albaPipeVolumeSliceBlend *pipe = (albaPipeVolumeSliceBlend *)m_CurrentVolume->GetPipe();
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void albaViewSliceBlend::GetLutRange(double minMax[2])
{
	if (!m_CurrentVolume)
		return;

	albaString pipe_name = m_CurrentVolume->GetPipe()->GetTypeName();
	if (pipe_name.Equals("albaPipeVolumeSliceBlend"))
	{
		albaPipeVolumeSliceBlend *pipe = (albaPipeVolumeSliceBlend *)m_CurrentVolume->GetPipe();
		pipe->GetLutRange(minMax);
	}
}

//----------------------------------------------------------------------------
void albaViewSliceBlend::SetSliceLocalOrigin(double origin0[3],double origin1[3])
{
  //If a volume is visualized set slice position
  if(m_CurrentVolume)
  {
    memcpy(m_Slice1,origin0,sizeof(origin0));
    memcpy(m_Slice2,origin1,sizeof(origin1));
		albaPipe * pipe = m_CurrentVolume->GetPipe();
    albaString pipe_name = pipe->GetTypeName();
    if (pipe_name.Equals("albaPipeVolumeSliceBlend"))
    {
      albaPipeVolumeSliceBlend *pipe = (albaPipeVolumeSliceBlend *)pipe;
      //Set origin0 for slice 0
      pipe->SetSlice(0,origin0);
      //Set origin1 for slice 1
      pipe->SetSlice(1,origin1);
    }
  }
}
//----------------------------------------------------------------------------
void albaViewSliceBlend::SetSlice(int nSlice,double pos[3])  
{
  if(nSlice>1 || nSlice<0)
    return;

  if(m_CurrentVolume)
  {
    //Check wicth slices to set position
    if(nSlice==0)
      memcpy(m_Slice1,pos,sizeof(m_Slice1));
    else if(nSlice==1)
      memcpy(m_Slice2,pos,sizeof(m_Slice2));

		albaPipe * pipe = m_CurrentVolume->GetPipe();
    albaString pipe_name = pipe->GetTypeName();
    if (pipe_name.Equals("albaPipeVolumeSliceBlend"))
    {
      albaPipeVolumeSliceBlend *pipeBlend = (albaPipeVolumeSliceBlend *)pipe;
      pipeBlend->SetSlice(nSlice,pos);
    }
  }
}
//----------------------------------------------------------------------------
double* albaViewSliceBlend::GetSlice(int nSlice)
{
	if (nSlice == 0)
		return m_Slice1;
	
	return m_Slice2;
}

//----------------------------------------------------------------------------
void albaViewSliceBlend::BorderCreate(double col[3])
{
  if(m_Border) 
  {
    //Before create a new border delete previous border
    BorderDelete();
  }

  vtkPlaneSource *ps = vtkPlaneSource::New();
  ps->SetOrigin(0, 0, 0);
  ps->SetPoint1(1, 0, 0);
  ps->SetPoint2(0, 1, 0);

  vtkOutlineFilter *of = vtkOutlineFilter::New();
  of->SetInputConnection(ps->GetOutputPort());

  vtkCoordinate *coord = vtkCoordinate::New();
  coord->SetCoordinateSystemToNormalizedViewport();
  coord->SetValue(1, 1, 0);

  vtkPolyDataMapper2D *pdmd = vtkPolyDataMapper2D::New();
  pdmd->SetInputConnection(of->GetOutputPort());
  pdmd->SetTransformCoordinate(coord);

  vtkProperty2D *pd = vtkProperty2D::New();
  pd->SetDisplayLocationToForeground();
  pd->SetLineWidth(3);
  pd->SetColor(col[0],col[1],col[2]);

  m_Border = vtkActor2D::New();
  m_Border->SetMapper(pdmd);
  m_Border->SetProperty(pd);
  m_Border->SetPosition(0,0);

  //Add border to ren front
  m_Rwi->m_RenFront->AddActor(m_Border);

  vtkDEL(ps);
  vtkDEL(of);
  vtkDEL(coord);
  vtkDEL(pdmd);
  vtkDEL(pd);
}
//----------------------------------------------------------------------------
void albaViewSliceBlend::BorderDelete()
{
  if(m_Border)
  {
    //Remove border from ren front
    m_Rwi->m_RenFront->RemoveActor(m_Border);
    vtkDEL(m_Border);
  }  
}

//----------------------------------------------------------------------------
void albaViewSliceBlend::UpdateSurfacesList(albaVME *vme)  
{
  // Remove node form surface list
  for(int i=0;i<m_CurrentSurface.size();i++)
  {
    if (m_CurrentSurface[i]==m_Sg->Vme2Node(vme))
    {
      std::vector<albaSceneNode*>::iterator startIterator;
      m_CurrentSurface.erase(m_CurrentSurface.begin()+i);
    }
  }
}

//----------------------------------------------------------------------------
void albaViewSliceBlend::VmeShow(albaVME *vme, bool show)  
{
  if (vme->IsALBAType(albaVMEVolume))
  {
    if (show)
    {
      // Attach the camera to the volume visualized
      if(m_AttachCamera)
        m_AttachCamera->SetVme(vme);
    }
    else
    {
      //detach volume from the camera
      if(m_AttachCamera)
        m_AttachCamera->SetVme(NULL);
    }
  }

  Superclass::VmeShow(vme, show);

  if (m_CurrentVolume!=NULL)
  {
    // if a Volume is present use his bounds for slices positions

    double b[6];
    albaVMEVolumeGray::SafeDownCast(m_CurrentVolume->GetVme())->GetOutput()->GetBounds(b);
    m_Slice1Position = b[4];
    m_Slice2Position = b[5];

    m_Slice1[2] = m_Slice1Position;
    SetSlice(0,m_Slice1);
    m_Slice2[2] = m_Slice2Position;
    SetSlice(1,m_Slice2);
    CameraUpdate();

    m_Gui->Update();
  }
  else
  {
    //Otherwise use 0,1 as range 
    m_Slice1Position = 0;
    m_Slice2Position = 1;

    m_Gui->Update();
  }

  m_Gui->Enable(ID_OPACITY,m_CurrentVolume!=NULL);
}
//----------------------------------------------------------------------------
void albaViewSliceBlend::VmeRemove(albaVME *vme)
{
  if(vme->IsA("albaVMEPolyline")||vme->IsA("albaVMESurface")||vme->IsA("albaVMEPolylineEditor"))
  {
    //remove surfaces from surfaces list
    this->UpdateSurfacesList(vme);
  }
  Superclass::VmeRemove(vme);
}

//-------------------------------------------------------------------------
void albaViewSliceBlend::Print(std::ostream& os, const int tabs)// const
{
  albaIndent indent(tabs);

  os << indent << "albaViewSliceBlend" << '\t' << this << std::endl;
  os << indent << "Name" << '\t' << m_Label << std::endl;
  os << std::endl;
  m_Sg->Print(os,1);
}
//-------------------------------------------------------------------------
void albaViewSliceBlend::SetNormal(double normal[3])
{
  //set normal to the surfaces pipes
  if(!m_CurrentSurface.empty())
  {
    for(int i=0;i<m_CurrentSurface.size();i++)
    {
			albaPipe * curSurfPipe = m_CurrentSurface.at(i)->GetPipe();
      if(curSurfPipe)
      {
        albaString pipe_name = curSurfPipe->GetTypeName();
        if (pipe_name.Equals("albaPipeSurfaceSlice"))
        {
          albaPipeSurfaceSlice *pipe = (albaPipeSurfaceSlice *)curSurfPipe;
          pipe->SetNormal(normal); 
        }
      }
    }
  }
}
//-------------------------------------------------------------------------
void albaViewSliceBlend::MultiplyPointByInputVolumeABSMatrix(double *point)
{
  if(m_CurrentVolume && m_CurrentVolume->GetVme())
  {
    albaMatrix *mat = m_CurrentVolume->GetVme()->GetOutput()->GetMatrix();
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
void albaViewSliceBlend::SetOpacity(double opacity)
{
	if (m_CurrentVolume)
	{
		if (opacity == 1.0) 
			opacity = 0.99;

		albaPipeVolumeSliceBlend *pipe = (albaPipeVolumeSliceBlend *)m_CurrentVolume->GetPipe();
		pipe->SetSliceOpacity(opacity);
		CameraUpdate();
	}
}