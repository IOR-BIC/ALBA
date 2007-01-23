/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSingleSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2007-01-23 15:37:11 $
  Version:   $Revision: 1.4 $
  Authors:   Daniele Giunchi
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

#include "mafViewSingleSlice.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipePolylineSlice.h"
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
#include "mmgFloatSlider.h"
#include "mafNodeIterator.h"

#include "vtkDataSet.h"
#include "vtkRayCast3DPicker.h"
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
mafViewSingleSlice::mafViewSingleSlice(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid, show_ruler, stereo)
//----------------------------------------------------------------------------
{
  m_CurrentVolume = NULL;
  m_Border        = NULL;
  
  m_Slice[0] = m_Slice[1] = m_Slice[2] = 0.0;
  m_SliceInitialized = false;

//  m_TextActor=NULL;
//  m_TextMapper=NULL;
//  m_TextColor[0]=1;
//  m_TextColor[1]=0;
//  m_TextColor[2]=0;

	m_Position = 0;
	m_PlaneSelect = XY;
	m_Slider   = NULL;
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
//  vtkDEL(m_TextMapper);
//  vtkDEL(m_TextActor);
  m_CurrentSurface.clear();
	m_CurrentPolyline.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewSingleSlice::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewSingleSlice *v = new mafViewSingleSlice(m_Label, m_CameraPosition, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::Create()
//----------------------------------------------------------------------------
{
  RWI_LAYERS num_layers = m_CameraPosition != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.001);
  m_Picker2D->InitializePickList();

  // text stuff
//  m_Text = "";
//  m_TextMapper = vtkTextMapper::New();
//  m_TextMapper->SetInput(m_Text.c_str());
//  m_TextMapper->GetTextProperty()->AntiAliasingOff();

//  m_TextActor = vtkActor2D::New();
//  m_TextActor->SetMapper(m_TextMapper);
//  m_TextActor->SetPosition(3,3);
//  m_TextActor->GetProperty()->SetColor(m_TextColor);

//  m_Rwi->m_RenFront->AddActor(m_TextActor);
}



//----------------------------------------------------------------------------
void mafViewSingleSlice::SetTextColor(double color[3])
//----------------------------------------------------------------------------
{
  /*m_TextColor[0]=color[0];
  m_TextColor[1]=color[1];
  m_TextColor[2]=color[2];
  m_TextActor->GetProperty()->SetColor(m_TextColor);
  m_TextMapper->Modified();*/
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::UpdateText(int ID)
//----------------------------------------------------------------------------
{
  /*if (ID==1)
  {
    int slice_mode;
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
    //set the init coordinates value
    if(slice_mode == SLICE_X)
      m_Text = "X = ";
    else if(slice_mode == SLICE_Y)
      m_Text = "Y = ";
    else if(slice_mode == SLICE_Z)
      m_Text = "Z = ";

    if((slice_mode != SLICE_ORTHO) && (slice_mode != SLICE_ARB))
      m_Text += wxString::Format("%.1f",m_Slice[slice_mode]);

    m_TextMapper->SetInput(m_Text.c_str());
    m_TextMapper->Modified();
  }
  else
  {
    m_Text="";
    m_TextMapper->SetInput(m_Text.c_str());
    m_TextMapper->Modified();
  }*/
}

//----------------------------------------------------------------------------
void mafViewSingleSlice::InitializeSlice(double slice[3])
//----------------------------------------------------------------------------
{
  memcpy(m_Slice,slice,sizeof(m_Slice));
  m_SliceInitialized = true;
}
//----------------------------------------------------------------------------
void mafViewSingleSlice::VmeCreatePipe(mafNode *vme)
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
      if (pipe_name.Equals("mafPipeVolumeSlice"))
      {
        m_CurrentVolume = n;
        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->m_Vme);
        int slice_mode;
        vtkDataSet *data = ((mafVME *)vme)->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
				float v1[3] = {1,0,0};
				float v2[3] = {0,1,0};
        switch(m_CameraPosition)
        {
          case CAMERA_OS_X:
            slice_mode = SLICE_X;
            v1[0] = 0;v1[1] = 1;v1[2] = 0;
						v2[0] = 0;v2[1] = 0;v2[2] = 1;
					break;
          case CAMERA_OS_Y:
            slice_mode = SLICE_Y;
						v1[0] = 0;v1[1] = 0;v1[2] = 1;
						v2[0] = 1;v2[1] = 0;v2[2] = 0;
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
      else if(pipe_name.Equals("mafPipeSurfaceSlice"))
      {
        double normal[3];
				switch(m_CameraPosition)
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
		    ((mafPipeSurfaceSlice *)pipe)->SetSlice(m_Slice);
				((mafPipeSurfaceSlice *)pipe)->SetNormal(normal);
      }
			else if(pipe_name.Equals("mafPipePolylineSlice"))
			{
				double normal[3];
				switch(m_CameraPosition)
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
				((mafPipePolylineSlice *)pipe)->SetSlice(m_Slice);
				((mafPipePolylineSlice *)pipe)->SetNormal(normal);
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
void mafViewSingleSlice::VmeDeletePipe(mafNode *vme)
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
int mafViewSingleSlice::GetNodeStatus(mafNode *vme)
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
    else if (vme->IsMAFType(mafVMEImage))
    {
      n = m_Sg->Vme2Node(vme);
      n->m_Mutex = true;
    }
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mmgGui *mafViewSingleSlice::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  //m_Gui->AddGui(m_AttachCamera->GetGui());

	m_Slider = m_Gui->FloatSlider(ID_POSITION, _("Position"), &m_Position,MINDOUBLE,MAXDOUBLE);

	const wxString plane_string[] = {_("XY"), _("YZ"), _("ZX")};
	m_Gui->Combo(ID_PLANE_SELECT, "View", &m_PlaneSelect, 3, plane_string);
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
					((mafPipeVolumeSlice *)m_CurrentVolume->m_Pipe)->GetSliceOrigin(m_OriginVolume);
					if(m_PlaneSelect == XY)
					{
						m_OriginVolume[2] = m_Position;
					}
					else if(m_PlaneSelect == YZ)
					{
						m_OriginVolume[0] = m_Position;
					}
					else if(m_PlaneSelect == ZX)
					{
						m_OriginVolume[1] = m_Position;
					}
					
					((mafPipeVolumeSlice *)m_CurrentVolume->m_Pipe)->SetSlice(m_OriginVolume);
					CameraUpdate();
				}
				else
					return;
				mafNodeIterator *iter = m_CurrentVolume->m_Vme->GetRoot()->NewIterator();
				for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
				{
					if(node->IsA("mafVMESurface"))
					{
						mafPipe *p= this->GetNodePipe(node);
						if(p)
							((mafPipeSurfaceSlice *)p)->SetSlice(m_OriginVolume);
					}
					if(node->IsA("mafVMEPolyline"))
					{
						mafPipe *p= this->GetNodePipe(node);
						if(p)
							((mafPipePolylineSlice *)p)->SetSlice(m_OriginVolume);
					}
				}
			}
		break;
		case ID_PLANE_SELECT:
		{
			double b[6];
			((mafVME *)m_CurrentVolume->m_Vme)->GetOutput()->GetBounds(b);


			if(m_PlaneSelect == XY)
			{
				m_CameraPosition = CAMERA_OS_Z;
				m_Position = (b[5] + b[4])/2;
				m_Slider->SetRange(b[4],b[5],m_Position);
				m_Slice[2]=m_Position;
			}
			else if(m_PlaneSelect == YZ)
			{
				m_CameraPosition = CAMERA_OS_X;
				m_Position = (b[1] + b[0])/2;
				m_Slider->SetRange(b[0],b[1],m_Position);
				m_Slice[0]=m_Position;
			}
			else if(m_PlaneSelect == ZX)
			{
				m_CameraPosition = CAMERA_OS_Y;
				m_Position = (b[3] + b[2])/2;
				m_Slider->SetRange(b[2],b[3],m_Position);
				m_Slice[1]=m_Position;
			}
			m_Rwi->CameraSet(m_CameraPosition);
			m_Gui->Enable(ID_POSITION,true);
			m_Gui->Update();


			mafNodeIterator *iter = m_CurrentVolume->m_Vme->GetRoot()->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface") || node->IsA("mafVMEVolume") || node->IsA("mafVMEPolyline"))
				{
					mafSceneNode *n = m_Sg->Vme2Node(node);
					if(n && n->IsVisible())
					{
						VmeDeletePipe(node);
						VmeCreatePipe(node);
					}
				}
			}
			
			CameraUpdate();
		}
		break;
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
void mafViewSingleSlice::SetLutRange(double low_val, double high_val)
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
void mafViewSingleSlice::SetSlice(double origin[3])
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume)
    return;
  memcpy(m_Slice,origin,sizeof(m_Slice));
  mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeSlice"))
  {
    mafPipeVolumeSlice *pipe = (mafPipeVolumeSlice *)m_CurrentVolume->m_Pipe;
    pipe->SetSlice(origin); 

    // update text
    this->UpdateText();
  }
  
  if(m_CurrentSurface.empty())
    return;
  for(int i=0;i<m_CurrentSurface.size();i++)
  {
    pipe_name = m_CurrentSurface.at(i)->m_Pipe->GetTypeName();
    if (pipe_name.Equals("mafPipeSurfaceSlice"))
    {
      mafPipeSurfaceSlice *pipe = (mafPipeSurfaceSlice *)m_CurrentSurface[i]->m_Pipe;
      pipe->SetSlice(origin); 
    }
  }

	if(m_CurrentPolyline.empty())
		return;
	for(int i=0;i<m_CurrentPolyline.size();i++)
	{
		pipe_name = m_CurrentPolyline.at(i)->m_Pipe->GetTypeName();
		if (pipe_name.Equals("mafPipePolylineSlice"))
		{
			mafPipePolylineSlice *pipe = (mafPipePolylineSlice *)m_CurrentPolyline[i]->m_Pipe;
			pipe->SetSlice(origin); 
		}
	}
  // update text
  this->UpdateText();
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
void mafViewSingleSlice::UpdateSurfacesList(mafNode *node)
//----------------------------------------------------------------------------
{
  for(int i=0;i<m_CurrentSurface.size();i++)
  {
    if (m_CurrentSurface[i]==m_Sg->Vme2Node(node))
    {
      std::vector<mafSceneNode*>::iterator startIterator;
      m_CurrentSurface.erase(m_CurrentSurface.begin()+i);
    }
  }

	for(int i=0;i<m_CurrentPolyline.size();i++)
	{
		if (m_CurrentPolyline[i]==m_Sg->Vme2Node(node))
		{
			std::vector<mafSceneNode*>::iterator startIterator;
			m_CurrentPolyline.erase(m_CurrentPolyline.begin()+i);
		}
	}
}

//----------------------------------------------------------------------------
void mafViewSingleSlice::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  Superclass::VmeShow(node, show);

  if (node->IsMAFType(mafVMEVolume))
  {
    if (show)
    {
      /*m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
      double sr[2],center[3];
      vtkDataSet *data = m_CurrentVolume->GetOutput()->GetVTKData();
      data->Update();
      data->GetCenter(center);
      data->GetScalarRange(sr);*/

			((mafPipeVolumeSlice *)m_CurrentVolume->m_Pipe)->GetSliceOrigin(m_OriginVolume);
      double b[6];
			((mafVME *)m_CurrentVolume->m_Vme)->GetOutput()->GetBounds(b);

			if(m_PlaneSelect == XY)
			{
				m_CameraPosition = CAMERA_OS_Z;
			  m_Position = (b[5] + b[4])/2;
				m_Slider->SetRange(b[4],b[5],m_Position);
				m_Slice[2]=m_Position;
			}
			else if(m_PlaneSelect == YZ)
			{
				m_CameraPosition = CAMERA_OS_X;
				m_Position = (b[1] + b[0])/2;
				m_Slider->SetRange(b[0],b[1],m_Position);
				m_Slice[0]=m_Position;
			}
			else if(m_PlaneSelect == ZX)
			{
				m_CameraPosition = CAMERA_OS_Y;
				m_Position = (b[3] + b[2])/2;
				m_Slider->SetRange(b[2],b[3],m_Position);
				m_Slice[1]=m_Position;
			}
			
			m_Gui->Enable(ID_POSITION,true);
			m_Gui->Update();

      /*m_Luts->SetRange((long)sr[0],(long)sr[1]);
      m_Luts->SetSubRange((long)sr[0],(long)sr[1]);
      vtkNEW(m_ColorLUT);
      m_ColorLUT->SetRange(sr);
      m_ColorLUT->Build();
      lutPreset(4,m_ColorLUT);*/
			CameraUpdate();
    }
    else
    {
  /*    m_CurrentVolume->GetEventSource()->RemoveObserver(this);
      m_CurrentVolume = NULL;
      for(int i=0; i<m_NumOfChildView; i++)
        ((mafViewSingleSliceLHPBuilder *)m_ChildViewList[i])->UpdateText(0);
	*/  
			mafNodeIterator *iter = node->GetRoot()->NewIterator();
			for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
			{
				if(node->IsA("mafVMESurface"))
				{
					mafEventMacro(mafEvent(this,VME_SHOW,node,false));
				}
				if(node->IsA("mafVMEPolyline"))
				{
					mafEventMacro(mafEvent(this,VME_SHOW,node,false));
				}
			}
      this->UpdateText(0);
			m_Gui->Enable(ID_POSITION,false);
			m_Gui->Update();

    }
  }

}
