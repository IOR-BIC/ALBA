/*=========================================================================

 Program: MAF2
 Module: mafViewSingleSlice
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

#include "mafViewSingleSlice.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMESurface.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"
#include "mafGUIFloatSlider.h"
#include "mafVMEIterator.h"
#include "mafVMEGizmo.h"
#include "mafPipeMeshSlice.h"
#include "mafGUI.h"

#include "vtkDataSet.h"
#include "vtkMAFRayCast3DPicker.h"
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


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSingleSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewSingleSlice::mafViewSingleSlice(wxString label, int camera_position, bool show_axes, bool show_grid, int stereo, bool show_orientation)
:mafViewVTK(label,camera_position,show_axes,show_grid, stereo, show_orientation)
//----------------------------------------------------------------------------
{
  m_CurrentVolume = NULL;
  m_Border        = NULL;
  
  m_Slice[0] = m_Slice[1] = m_Slice[2] = 0.0;
  m_SliceInitialized = false;

  m_TextActor=NULL;
  m_TextMapper=NULL;
  m_TextColor[0]=1;
  m_TextColor[1]=0;
  m_TextColor[2]=0;

	m_Position = 0;
	m_PlaneSelect = XY;
	//m_Slider   = NULL;
	m_OriginVolume[0] = 0.0;
	m_OriginVolume[1] = 0.0;
  m_OriginVolume[2] = 0.0;
  m_CurrentSurface.clear();
	m_CurrentPolyline.clear();
}
//----------------------------------------------------------------------------
mafViewSingleSlice::~mafViewSingleSlice()
//----------------------------------------------------------------------------
{
  BorderDelete();
  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);
  m_CurrentSurface.clear();
	m_CurrentPolyline.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewSingleSlice::Copy(mafObserver *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewSingleSlice *v = new mafViewSingleSlice(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_StereoType,m_ShowOrientator);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_LightCopyEnabled = lightCopyEnabled;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::Create()
//----------------------------------------------------------------------------
{

  if(m_LightCopyEnabled) return;

  RWI_LAYERS num_layers = TWO_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_StereoType, m_ShowOrientator);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack, m_Rwi->m_AlwaysVisibleRenderer);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.0001);
  m_Picker2D->InitializePickList();

  // text stuff
  m_Text = "";
  m_TextMapper = vtkTextMapper::New();
  m_TextMapper->SetInput(m_Text.c_str());
  m_TextMapper->GetTextProperty()->AntiAliasingOff();

  m_TextActor = vtkActor2D::New();
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->SetPosition(3,3);
  m_TextActor->GetProperty()->SetColor(m_TextColor);

  m_Rwi->m_RenFront->AddActor(m_TextActor);
}



//----------------------------------------------------------------------------
void mafViewSingleSlice::SetTextColor(double color[3])
//----------------------------------------------------------------------------
{
  m_TextColor[0]=color[0];
  m_TextColor[1]=color[1];
  m_TextColor[2]=color[2];
  m_TextActor->GetProperty()->SetColor(m_TextColor);
  m_TextMapper->Modified();
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::UpdateText(int ID)
//----------------------------------------------------------------------------
{
  if (ID==1)
  {
    int slice_mode;
    switch(m_CameraPositionId)
    {
    case CAMERA_OS_X:
      slice_mode = mafPipeVolumeSlice::SLICE_X;
      break;
    case CAMERA_OS_Y:
      slice_mode = mafPipeVolumeSlice::SLICE_Y;
      break;
    case CAMERA_OS_P:
      slice_mode = mafPipeVolumeSlice::SLICE_ORTHO;
      break;
    case CAMERA_PERSPECTIVE:
      slice_mode = mafPipeVolumeSlice::SLICE_ARB;
      break;
    default:
      slice_mode = mafPipeVolumeSlice::SLICE_Z;
    }
    //set the init coordinates value
    if(slice_mode == mafPipeVolumeSlice::SLICE_X)
      m_Text = "X: ";
    else if(slice_mode == mafPipeVolumeSlice::SLICE_Y)
      m_Text = "Y: ";
    else if(slice_mode == mafPipeVolumeSlice::SLICE_Z)
      m_Text = "Z: ";

    if((slice_mode != mafPipeVolumeSlice::SLICE_ORTHO) && (slice_mode != mafPipeVolumeSlice::SLICE_ARB))
      m_Text += wxString::Format("%.1f",m_Slice[slice_mode]);

    m_TextMapper->SetInput(m_Text.c_str());
    m_TextMapper->Modified();
  }
  else
  {
    m_Text="";
    m_TextMapper->SetInput(m_Text.c_str());
    m_TextMapper->Modified();
  }
  if(m_TextActor)
  {
    wxSize size = ((wxWindowBase*)this->GetRWI())->GetSize();
    m_TextActor->SetPosition(size.GetWidth() - 80, size.GetHeight() - 25);
  }
}

//----------------------------------------------------------------------------
void mafViewSingleSlice::InitializeSlice(double slice[3])
//----------------------------------------------------------------------------
{
  memcpy(m_Slice,slice,sizeof(m_Slice));
  m_SliceInitialized = true;
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::VmeCreatePipe(mafVME *vme)
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
      if (pipe_name.Equals("mafPipeVolumeSlice"))
      {
        m_CurrentVolume = n;
        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->GetVme());
        int slice_mode;
        vtkDataSet *data = vme->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
				float v1[3] = {1,0,0};
				float v2[3] = {0,1,0};
        switch(m_CameraPositionId)
        {
          case CAMERA_OS_X:
            slice_mode = mafPipeVolumeSlice::SLICE_X;
            v1[0] = 0;v1[1] = 1;v1[2] = 0;
						v2[0] = 0;v2[1] = 0;v2[2] = 1;
					break;
          case CAMERA_OS_Y:
            slice_mode = mafPipeVolumeSlice::SLICE_Y;
						v1[0] = 0;v1[1] = 0;v1[2] = 1;
						v2[0] = 1;v2[1] = 0;v2[2] = 0;
          break;
          case CAMERA_OS_P:
            slice_mode = mafPipeVolumeSlice::SLICE_ORTHO;
          break;
          case CAMERA_PERSPECTIVE:
            slice_mode = mafPipeVolumeSlice::SLICE_ARB;
          break;
          default:
            slice_mode = mafPipeVolumeSlice::SLICE_Z;
        }
        if (m_SliceInitialized)
        {
          ((mafPipeVolumeSlice *)pipe)->InitializeSliceParameters(slice_mode,m_Slice,false);
					((mafPipeVolumeSlice *)pipe)->SetSlice(m_Slice,v1,v2);
        }
        else
        {
          ((mafPipeVolumeSlice *)pipe)->InitializeSliceParameters(slice_mode,false);
        }
        UpdateText();
      }
      else if(pipe_name.Equals("mafPipeSurfaceSlice_BES"))
      {
        double normal[3];
				switch(m_CameraPositionId)
				{
				case CAMERA_OS_X:
					normal[0] = 1;
				  normal[1] = 0;
 					normal[2] = 0;
					break;
				case CAMERA_OS_Y:
					normal[0] = 0;
					normal[1] = 1;
					normal[2] = 0;
					break;
				case CAMERA_OS_Z:
					normal[0] = 0;
					normal[1] = 0;
					normal[2] = 1;
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

		    m_CurrentSurface.push_back(n);
		    ((mafPipeSurfaceSlice_BES *)pipe)->SetSlice(m_Slice,normal);
      }
			else if(pipe_name.Equals("mafPipeMeshSlice"))
			{
				double normal[3];
				switch(m_CameraPositionId)
				{
				case CAMERA_OS_X:
					normal[0] = 1;
					normal[1] = 0;
					normal[2] = 0;
					break;
				case CAMERA_OS_Y:
					normal[0] = 0;
					normal[1] = 1;
					normal[2] = 0;
					break;
				case CAMERA_OS_Z:
					normal[0] = 0;
					normal[1] = 0;
					normal[2] = 1;
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

				((mafPipeMeshSlice *)pipe)->SetSlice(m_Slice, normal);
			}
			else if(pipe_name.Equals("mafPipePolylineSlice_BES"))
			{
				double normal[3];
				switch(m_CameraPositionId)
				{
				case CAMERA_OS_X:
					normal[0] = 1;
					normal[1] = 0;
					normal[2] = 0;
					break;
				case CAMERA_OS_Y:
					normal[0] = 0;
					normal[1] = 1;
					normal[2] = 0;
					break;
				case CAMERA_OS_Z:
					normal[0] = 0;
					normal[1] = 0;
					normal[2] = 1;
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

				m_CurrentPolyline.push_back(n);
				((mafPipePolylineSlice_BES *)pipe)->SetSlice(m_Slice,normal);
			}
			pipe->Create(n);

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
void mafViewSingleSlice::VmeDeletePipe(mafVME *vme)
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
int mafViewSingleSlice::GetNodeStatus(mafVME *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  if (m_Sg != NULL)
  {
    n = m_Sg->Vme2Node(vme);
    if (vme->GetOutput()->IsA("mafVMEOutputVolume") || vme->IsMAFType(mafVMEImage) || vme->IsMAFType(mafVMEPolylineSpline))
    {
      if(n != NULL)
        n->SetMutex(true);
    }
    else if (vme->IsMAFType(mafVMEPolyline) || vme->IsMAFType(mafVMESurface))
    {
      if(n != NULL)
        n->SetMutex(false);
    }
    else if (vme->IsMAFType(mafVMESlicer) || vme->GetOutput()->IsA("mafVMEAdvancedProber"))
    {
      if(n != NULL)
        n->SetPipeCreatable(false);
    }
		else if (vme->IsMAFType(mafVMEGizmo))
		{
      if(n != NULL)
			  n->SetPipeCreatable(true);
		}
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mafGUI *mafViewSingleSlice::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  //m_Gui->AddGui(m_AttachCamera->GetGui());

	//m_Slider = m_Gui->FloatSlider(ID_POSITION, _("Position"), &m_Position,MINDOUBLE,MAXDOUBLE);
  m_Gui->Double(ID_POSITION, _("Position"), &m_Position,MINDOUBLE,MAXDOUBLE,2);
	m_Gui->Enable(ID_POSITION,false);

	//const wxString plane_string[] = {_("XY"), _("YZ"), _("ZX")};
	//m_Gui->Combo(ID_PLANE_SELECT, "View", &m_PlaneSelect, 3, plane_string);
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
		case ID_POSITION:
			{
				if (m_CurrentVolume)
				{
					mafPipeVolumeSlice * pipe = (mafPipeVolumeSlice *)m_CurrentVolume->GetPipe();
					pipe->GetOrigin(m_OriginVolume);
					if(m_PlaneSelect == XY)
					{
						m_OriginVolume[2] = m_Position;
            m_Slice[2] = m_Position;
					}
					else if(m_PlaneSelect == YZ)
					{
						m_OriginVolume[0] = m_Position;
            m_Slice[0] = m_Position;
					}
					else if(m_PlaneSelect == ZX)
					{
						m_OriginVolume[1] = m_Position;
            m_Slice[1] = m_Position;
					}
					
					pipe->SetOrigin(m_OriginVolume);
          
          this->UpdateText();
          CameraUpdate();
				}
				else
					return;
				mafVMEIterator *iter = m_CurrentVolume->GetVme()->GetRoot()->NewIterator();
				for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
				{
					mafPipeSlice *pipeSlice = mafPipeSlice::SafeDownCast(this->GetNodePipe(node));
					if(pipeSlice)
							pipeSlice->SetSlice(m_OriginVolume,NULL);
				}
        iter->Delete();
			}
      mafEventMacro(mafEvent(this,ID_POSITION,m_Position));
		break;
		case ID_PLANE_SELECT:
		{
			double b[6];
			m_CurrentVolume->GetVme()->GetOutput()->GetBounds(b);


			if(m_PlaneSelect == XY)
			{
				m_CameraPositionId = CAMERA_OS_Z;
				m_Position = (b[5] + b[4])/2;
				//m_Slider->SetRange(b[4],b[5],m_Position);
				m_Slice[2]=m_Position;
			}
			else if(m_PlaneSelect == YZ)
			{
				m_CameraPositionId = CAMERA_OS_X;
				m_Position = (b[1] + b[0])/2;
				//m_Slider->SetRange(b[0],b[1],m_Position);
				m_Slice[0]=m_Position;
			}
			else if(m_PlaneSelect == ZX)
			{
				m_CameraPositionId = CAMERA_OS_Y;
				m_Position = (b[3] + b[2])/2;
				//m_Slider->SetRange(b[2],b[3],m_Position);
				m_Slice[1]=m_Position;
			}
			m_Rwi->CameraSet(m_CameraPositionId);
			m_Gui->Enable(ID_POSITION,true);
			m_Gui->Update();


			mafVMEIterator *iter = m_CurrentVolume->GetVme()->GetRoot()->NewIterator();
			for (mafVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
			  if(node->IsA("mafVMESurface") || node->GetOutput()->IsA("mafVMEOutputVolume") || node->IsA("mafVMEPolyline"))
				{
					mafSceneNode *n = m_Sg->Vme2Node(node);
					if(n && n->IsVisible())
					{
						VmeDeletePipe(node);
						VmeCreatePipe(node);
					}
				}
			}
      iter->Delete();
			
			CameraUpdate();
		}
		break;
    default:
      Superclass::OnEvent(maf_event);
    break;
    }
  }
  else
  {
		Superclass::OnEvent(maf_event);
  }
  
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
	mafPipe * pipe = m_CurrentVolume->GetPipe();
  mafString pipe_name =pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeSlice"))
  {
    mafPipeVolumeSlice *pipe = (mafPipeVolumeSlice *)pipe;
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::SetSlice(double origin[3])
//----------------------------------------------------------------------------
{
	memcpy(m_Slice,origin,sizeof(m_Slice));
	mafString pipe_name;
  if(m_CurrentVolume)
	{
		mafPipe * pipe = m_CurrentVolume->GetPipe();
		pipe_name = pipe->GetTypeName();
		if (pipe_name.Equals("mafPipeVolumeSlice"))
		{
			mafPipeVolumeSlice *pipe = (mafPipeVolumeSlice *)pipe;
			pipe->SetOrigin(origin); 

			// update text
			this->UpdateText();
			CameraUpdate();
		}
	}
  
  //if(m_CurrentSurface.empty())
  //  return;
  for(int i=0;i<m_CurrentSurface.size();i++)
  {
		mafPipe * pipe = m_CurrentSurface.at(i)->GetPipe();
    pipe_name = pipe->GetTypeName();
    if (pipe_name.Equals("mafPipeSurfaceSlice_BES"))
    {
      mafPipeSurfaceSlice_BES *pipe = (mafPipeSurfaceSlice_BES *)m_CurrentSurface[i]->GetPipe();
      pipe->SetSlice(origin,NULL); 
    }
  }

	//if(m_CurrentPolyline.empty())
	//	return;
	for(int i=0;i<m_CurrentPolyline.size();i++)
	{
		mafPipe * pipe = m_CurrentPolyline.at(i)->GetPipe();
		pipe_name = pipe->GetTypeName();
		if (pipe_name.Equals("mafPipePolylineSlice_BES"))
		{
			mafPipePolylineSlice_BES *pipe = (mafPipePolylineSlice_BES *)pipe;
			pipe->SetSlice(origin,NULL); 
		}
	}
  // update text
  this->UpdateText();
  if(m_Gui)
  {
    if(m_PlaneSelect == XY)
    {
      m_Position = origin[2];
    }
    else if(m_PlaneSelect == YZ)
    {
      m_Position = origin[0];
    }
    else if(m_PlaneSelect == ZX)
    {
      m_Position = origin[1];
    }
    m_Gui->Update();
  }
	CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::GetSlice(double slice[3])
//----------------------------------------------------------------------------
{
  memcpy(slice,m_Slice,sizeof(m_Slice));
}
//----------------------------------------------------------------------------
double *mafViewSingleSlice::GetSlice()
//----------------------------------------------------------------------------
{
  return m_Slice;
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::BorderCreate(double col[3])
//----------------------------------------------------------------------------
{
  if(m_Border) BorderDelete();

  vtkPlaneSource *ps = vtkPlaneSource::New();
  ps->SetOrigin(0, 0, 0);
  ps->SetPoint1(1, 0, 0);
  ps->SetPoint2(0, 1, 0);

  vtkOutlineFilter *of = vtkOutlineFilter::New();
  of->SetInput((vtkDataSet *)ps->GetOutput());

  vtkCoordinate *coord = vtkCoordinate::New();
  coord->SetCoordinateSystemToNormalizedViewport();
  coord->SetValue(1, 1, 0);

  vtkPolyDataMapper2D *pdmd = vtkPolyDataMapper2D::New();
  pdmd->SetInput(of->GetOutput());
  pdmd->SetTransformCoordinate(coord);

  vtkProperty2D *pd = vtkProperty2D::New();
  pd->SetDisplayLocationToForeground();
  pd->SetLineWidth(3);
  pd->SetColor(col[0],col[1],col[2]);

  m_Border = vtkActor2D::New();
  m_Border->SetMapper(pdmd);
  m_Border->SetProperty(pd);
  m_Border->SetPosition(0,0);

  m_Rwi->m_RenFront->AddActor(m_Border);

  vtkDEL(ps);
  vtkDEL(of);
  vtkDEL(coord);
  vtkDEL(pdmd);
  vtkDEL(pd);
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::BorderDelete()
//----------------------------------------------------------------------------
{
  if(m_Border)
  {
    m_Rwi->m_RenFront->RemoveActor(m_Border);
    vtkDEL(m_Border);
  }  
}

//----------------------------------------------------------------------------
void mafViewSingleSlice::UpdateSurfacesList(mafVME *vme)
//----------------------------------------------------------------------------
{
  for(int i=0;i<m_CurrentSurface.size();i++)
  {
    if (m_CurrentSurface[i]==m_Sg->Vme2Node(vme))
    {
      std::vector<mafSceneNode*>::iterator startIterator;
      m_CurrentSurface.erase(m_CurrentSurface.begin()+i);
    }
  }

	for(int i=0;i<m_CurrentPolyline.size();i++)
	{
		if (m_CurrentPolyline[i]==m_Sg->Vme2Node(vme))
		{
			std::vector<mafSceneNode*>::iterator startIterator;
			m_CurrentPolyline.erase(m_CurrentPolyline.begin()+i);
		}
	}
}

//----------------------------------------------------------------------------
void mafViewSingleSlice::VmeShow(mafVME *vme, bool show)
//----------------------------------------------------------------------------
{
  Superclass::VmeShow(vme, show);

  if (vme->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (show)
    {
			((mafPipeVolumeSlice *)m_CurrentVolume->GetPipe())->GetOrigin(m_OriginVolume);
      double b[6];
			m_CurrentVolume->GetVme()->GetOutput()->GetBounds(b);

			if(m_PlaneSelect == XY)
			{
				m_CameraPositionId = CAMERA_OS_Z;
			  m_Position = (b[5] + b[4])/2;
				m_Slice[2]=m_Position;
			}
			else if(m_PlaneSelect == YZ)
			{
				m_CameraPositionId = CAMERA_OS_X;
				m_Position = (b[1] + b[0])/2;
				m_Slice[0]=m_Position;
			}
			else if(m_PlaneSelect == ZX)
			{
				m_CameraPositionId = CAMERA_OS_Y;
				m_Position = (b[3] + b[2])/2;
				m_Slice[1]=m_Position;
			}
			
			m_Gui->Enable(ID_POSITION,true);
			m_Gui->Update();

      this->UpdateText();
			CameraUpdate();
    }
    else
    {
			mafVMEIterator *iter = vme->GetRoot()->NewIterator();
			for (mafVME *Inode = iter->GetFirstNode(); Inode; Inode = iter->GetNextNode())
			{
				if(this->GetNodePipe(Inode))
				{
					if(Inode->IsA("mafVMESurface"))
					{
						mafEventMacro(mafEvent(this,VME_SHOW,Inode,false));
					}
					if(Inode->IsA("mafVMEPolyline"))
					{
						mafEventMacro(mafEvent(this,VME_SHOW,Inode,false));
					}
				}
			}
      iter->Delete();
      this->UpdateText(0);
			m_Gui->Enable(ID_POSITION,false);
			m_Gui->Update();
    }
  }
  else
  {
    if(!show)
      UpdateSurfacesList(vme);
  }
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::SetPickerTolerance(double tolerance)
//----------------------------------------------------------------------------
{
  m_Picker2D->SetTolerance(tolerance);
  m_Picker2D->Modified();
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::SetTextSize(int size)
//----------------------------------------------------------------------------
{
  m_TextMapper->GetTextProperty()->SetFontSize(size);
}
