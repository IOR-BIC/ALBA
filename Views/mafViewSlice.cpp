/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-14 11:18:45 $
  Version:   $Revision: 1.51.2.5 $
  Authors:   Paolo Quadrani,Stefano Perticoni, modified by Josef Kohout
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

#include "mafIndent.h"
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice_BES.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafPipeMeshSlice_BES.h"
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
#include "medPipePolylineGraphEditor.h"

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
#include "vtkCamera.h"
#include "vtkTransform.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSlice);
//----------------------------------------------------------------------------

#include "mafMemDbg.h"

//----------------------------------------------------------------------------
mafViewSlice::mafViewSlice(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo,bool showTICKs)
:mafViewVTK(label,camera_position,show_axes,show_grid, show_ruler, stereo)
//----------------------------------------------------------------------------
{
  m_CurrentVolume = NULL;
  m_Border        = NULL;
  
  m_Slice[0] = m_Slice[1] = m_Slice[2] = 0.0;
  InitializeSlice(m_Slice);     //to correctly set the normal  
  m_SliceInitialized = false;   //reset initialized to false 

  m_TextActor=NULL;
  m_TextMapper=NULL;
  m_TextColor[0]=1;
  m_TextColor[1]=0;
  m_TextColor[2]=0;

  m_CurrentSurface.clear();
	m_CurrentPolyline.clear();
	m_CurrentPolylineGraphEditor.clear();
  m_CurrentMesh.clear();

	m_ShowVolumeTICKs = showTICKs;
}
//----------------------------------------------------------------------------
mafViewSlice::~mafViewSlice()
//----------------------------------------------------------------------------
{
  BorderDelete();
  vtkDEL(m_TextMapper);
  vtkDEL(m_TextActor);
  m_CurrentSurface.clear();
	m_CurrentPolyline.clear();
	m_CurrentPolylineGraphEditor.clear();
  m_CurrentMesh.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewSlice::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewSlice *v = new mafViewSlice(m_Label, m_CameraPositionId, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType,m_ShowVolumeTICKs);
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
  RWI_LAYERS num_layers = m_CameraPositionId != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPositionId);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.005);
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
void mafViewSlice::SetTextColor(double color[3])
//----------------------------------------------------------------------------
{
  m_TextColor[0]=color[0];
  m_TextColor[1]=color[1];
  m_TextColor[2]=color[2];
  m_TextActor->GetProperty()->SetColor(m_TextColor);
  m_TextMapper->Modified();
}
//----------------------------------------------------------------------------
void mafViewSlice::UpdateText(int ID)
//----------------------------------------------------------------------------
{
  if (ID==1)
  {
    int slice_mode;
    switch(m_CameraPositionId)
    {
    case CAMERA_OS_X:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_X;
      break;
    case CAMERA_OS_Y:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_Y;
      break;
    case CAMERA_OS_P:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_ORTHO;
      break;
    case CAMERA_PERSPECTIVE:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_ARB;
      break;
		case CAMERA_ARB:
			slice_mode = mafPipeVolumeSlice_BES::SLICE_ARB;
			break;
    default:
      slice_mode = mafPipeVolumeSlice_BES::SLICE_Z;
    }
    //set the init coordinates value
    if(slice_mode == mafPipeVolumeSlice_BES::SLICE_X)
      m_Text = "X = ";
    else if(slice_mode == mafPipeVolumeSlice_BES::SLICE_Y)
      m_Text = "Y = ";
    else if(slice_mode == mafPipeVolumeSlice_BES::SLICE_Z)
      m_Text = "Z = ";

    if((slice_mode != mafPipeVolumeSlice_BES::SLICE_ORTHO) && (slice_mode != mafPipeVolumeSlice_BES::SLICE_ARB))
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
}

//----------------------------------------------------------------------------
//Give an initial origin and normal (optionally) for the slice.
void mafViewSlice::InitializeSlice(double* Origin, double* Normal)
//----------------------------------------------------------------------------
{
  memcpy(m_Slice,Origin,sizeof(m_Slice));
  if (Normal != NULL) {
    memcpy(m_SliceNormal,Normal,sizeof(m_SliceNormal));
  }
  else
  {
    if (m_CameraPositionId == CAMERA_ARB)
      this->GetRWI()->GetCamera()->GetViewPlaneNormal(m_SliceNormal);
    else
    {
      m_SliceNormal[0] = m_SliceNormal[1] = m_SliceNormal[2] = 0.0;

      switch(m_CameraPositionId)
      {    
      case CAMERA_OS_X:
        m_SliceNormal[0] = 1;      
        break;
      case CAMERA_OS_Y:      
        m_SliceNormal[1] = 1;      
        break;
        //case CAMERA_OS_Z:  
      default:
        m_SliceNormal[2] = 1;
        break;
      }
    }
  }

  m_SliceInitialized = true;
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
    if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || 
      vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 1) {
        m_NumberOfVisibleVme = 1;
    }
    else {
      m_NumberOfVisibleVme++;
    }
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe != NULL)
    {
      pipe->SetListener(this);
      if (pipe->IsA("mafPipeVolumeSlice_BES"))  //BES: 3.4.2009 - changed to support inheritance
      {
        m_CurrentVolume = n;
        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->m_Vme);
        int slice_mode;
        vtkDataSet *data = ((mafVME *)vme)->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
        switch(m_CameraPositionId)
        {
        case CAMERA_OS_X:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_X;
          break;
        case CAMERA_OS_Y:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_Y;
          break;
        case CAMERA_OS_P:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_ORTHO;
          break;
        case CAMERA_PERSPECTIVE:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_ARB;
          break;
        default:
          slice_mode = mafPipeVolumeSlice_BES::SLICE_Z;
        }
        if (m_SliceInitialized)
        {
          ((mafPipeVolumeSlice_BES *)pipe)->InitializeSliceParameters(slice_mode, m_Slice, false);
          ((mafPipeVolumeSlice_BES *)pipe)->SetNormal(m_SliceNormal);
        }
        else
        {
          ((mafPipeVolumeSlice_BES *)pipe)->InitializeSliceParameters(slice_mode,false);
        }

        if(m_ShowVolumeTICKs)
          ((mafPipeVolumeSlice_BES *)pipe)->ShowTICKsOn();
        else
          ((mafPipeVolumeSlice_BES *)pipe)->ShowTICKsOff();

        UpdateText();
      }
      else 
      {
        //not a VolumeSlice pipe, check, if it is some slicer
        mafPipeSlice* spipe = mafPipeSlice::SafeDownCast(pipe);
        if (spipe != NULL)
        { 
          //it is slicing pipe
          //initialize supported slicing pipes
          if(pipe->IsA("mafPipeSurfaceSlice_BES")){
            m_CurrentSurface.push_back(n);        
          }
          else if(pipe->IsA("mafPipePolylineSlice_BES")){
            m_CurrentPolyline.push_back(n);        
          }
          else if(pipe->IsA("medPipePolylineGraphEditor"))
          {
            m_CurrentPolylineGraphEditor.push_back(n);

            if(m_CameraPositionId==CAMERA_OS_P)
              ((medPipePolylineGraphEditor *)pipe)->SetModalityPerspective();
            else
              ((medPipePolylineGraphEditor *)pipe)->SetModalitySlice();				
          }
          else if(pipe->IsA("mafPipeMeshSlice_BES"))  {
            m_CurrentMesh.push_back(n);        
          }

          //common stuff
          double positionSlice[3];
          positionSlice[0] = m_Slice[0];
          positionSlice[1] = m_Slice[1];
          positionSlice[2] = m_Slice[2];
          VolumePositionCorrection(positionSlice);
          spipe->SetSlice(positionSlice, m_SliceNormal);
        }
      } //end else [it is not volume slicing]                     

      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
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
  if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || 
    vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 0)
    m_NumberOfVisibleVme = 0;
  else
    m_NumberOfVisibleVme--;
  
  if (((mafVME *)vme)->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    m_CurrentVolume = NULL;
    if (m_AttachCamera)
    {
      m_AttachCamera->SetVme(NULL);
    }
  }
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);

  if(vme->IsMAFType(mafVMELandmark))
    UpdateSurfacesList(vme);
}
//-------------------------------------------------------------------------
int mafViewSlice::GetNodeStatus(mafNode *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  if (m_Sg != NULL)
  {
    n = m_Sg->Vme2Node(vme);
     if (((mafVME *)vme)->GetOutput()->IsA("mafVMEOutputVolume") || 
         vme->IsMAFType(mafVMESlicer))
    {
      if (n != NULL)
      {
      	n->m_Mutex = true;
      }
    }
    else if (vme->IsMAFType(mafVMEImage))
    {
      //n->m_Mutex = true;
			if (n != NULL)
			{
				n->m_PipeCreatable = false;
			}
    }
  }

  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mafGUI *mafViewSlice::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafEventMacro(*maf_event);
}

//----------------------------------------------------------------------------
void mafViewSlice::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
  
  mafPipeVolumeSlice_BES *pipe = mafPipeVolumeSlice_BES::SafeDownCast(m_CurrentVolume->m_Pipe);
  if (pipe != NULL) {    
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void mafViewSlice::SetSlice(double* Origin, double* Normal)
//----------------------------------------------------------------------------
{
  //set slice origin and normal
  if (Origin != NULL)
    memcpy(m_Slice,Origin,sizeof(m_Slice));

  if (Normal != NULL)
    memcpy(m_SliceNormal,Normal,sizeof(m_Slice));

  //and now set it for every VME
  if (m_CurrentVolume)
	{
    mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(m_CurrentVolume->m_Pipe);
		if (pipe != NULL)
		{
			pipe->SetSlice(Origin, Normal); 

			// update text
			this->UpdateText();
		}
	}
  
  //BES 1.5.2008 - I don't understand why GetViewPlaneNormal is used
  //so I leave it untouched
  double normal[3];
  if (Normal != NULL)
    memcpy(normal,Normal,sizeof(m_Slice));
  else
  this->GetRWI()->GetCamera()->GetViewPlaneNormal(normal);
	
  double coord[3];
  coord[0]=Origin[0];
  coord[1]=Origin[1];
  coord[2]=Origin[2];
  VolumePositionCorrection(coord);

  for(int i = 0; i < m_CurrentSurface.size(); i++)
  {
    if (m_CurrentSurface.at(i) && m_CurrentSurface.at(i)->m_Pipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(m_CurrentSurface.at(i)->m_Pipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, normal); 
      }
    }
  }	


  for(int i = 0;i < m_CurrentPolyline.size();i++)
  {
    if(m_CurrentPolyline.at(i) && m_CurrentPolyline.at(i)->m_Pipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(m_CurrentPolyline.at(i)->m_Pipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, normal); 
      }
    }
  }	

	for(int i = 0; i < m_CurrentPolylineGraphEditor.size();i++)
	{
    if (m_CurrentPolylineGraphEditor.at(i) && m_CurrentPolylineGraphEditor.at(i)->m_Pipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(m_CurrentPolylineGraphEditor.at(i)->m_Pipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, normal); 
      }
    }   
	}

  for(int i = 0; i < m_CurrentMesh.size();i++)
  {
    if (m_CurrentMesh.at(i) && m_CurrentMesh.at(i)->m_Pipe)
    {
      mafPipeSlice* pipe = mafPipeSlice::SafeDownCast(m_CurrentMesh.at(i)->m_Pipe);
      if (pipe != NULL){
        pipe->SetSlice(coord, normal); 
      }
    }   
  }

  // update text
  this->UpdateText();
}
//----------------------------------------------------------------------------
//Get the slice origin coordinates and normal.
void mafViewSlice::GetSlice(double* Origin, double* Normal)
//----------------------------------------------------------------------------
{
  if (Origin != NULL)
    memcpy(Origin, m_Slice, sizeof(m_Slice));

  if (Normal != NULL)
    memcpy(Normal, m_SliceNormal, sizeof(m_SliceNormal));
}

//----------------------------------------------------------------------------
void mafViewSlice::BorderUpdate()
//----------------------------------------------------------------------------
{
  if (NULL != m_Border)
  {
    BorderCreate(m_BorderColor);
  }
}

//----------------------------------------------------------------------------
void mafViewSlice::BorderCreate(double col[3])
//----------------------------------------------------------------------------
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
  of->SetInput((vtkDataSet *)ps->GetOutput());

  vtkCoordinate *coord = vtkCoordinate::New();
  coord->SetCoordinateSystemToDisplay();
  coord->SetValue(size[0]-1, size[1]-1, 0);

  vtkPolyDataMapper2D *pdmd = vtkPolyDataMapper2D::New();
  pdmd->SetInput(of->GetOutput());
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
void mafViewSlice::SetBorderOpacity(double value)
//----------------------------------------------------------------------------
{
  if(m_Border)
  {
    m_Border->GetProperty()->SetOpacity(value);
    m_Border->Modified();
  }

}
//----------------------------------------------------------------------------
void mafViewSlice::BorderDelete()
//----------------------------------------------------------------------------
{
  if(m_Border)
  {
    m_Rwi->m_RenFront->RemoveActor(m_Border);
    vtkDEL(m_Border);
  }  
}

//----------------------------------------------------------------------------
void mafViewSlice::UpdateSurfacesList(mafNode *node)
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

	for(int i=0;i<m_CurrentPolylineGraphEditor.size();i++)
	{
		if (m_CurrentPolylineGraphEditor[i]==m_Sg->Vme2Node(node))
		{
			std::vector<mafSceneNode*>::iterator startIterator;
			m_CurrentPolylineGraphEditor.erase(m_CurrentPolylineGraphEditor.begin()+i);
		}
	}

  for(int i=0;i<m_CurrentMesh.size();i++)
  {
    if (m_CurrentMesh[i]==m_Sg->Vme2Node(node))
    {
      std::vector<mafSceneNode*>::iterator startIterator;
      m_CurrentMesh.erase(m_CurrentMesh.begin()+i);
    }
  }
}

//----------------------------------------------------------------------------
void mafViewSlice::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  if (((mafVME *)node)->GetOutput()->IsA("mafVMEOutputVolume"))
  {
    if (show)
    {
			if(m_AttachCamera)
				m_AttachCamera->SetVme(node);
		/*m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
      double sr[2],center[3];
      vtkDataSet *data = m_CurrentVolume->GetOutput()->GetVTKData();
      data->Update();
      data->GetCenter(center);
      data->GetScalarRange(sr);
      m_Luts->SetRange((long)sr[0],(long)sr[1]);
      m_Luts->SetSubRange((long)sr[0],(long)sr[1]);
      vtkNEW(m_ColorLUT);
      m_ColorLUT->SetRange(sr);
      m_ColorLUT->Build();
      lutPreset(4,m_ColorLUT);*/
	  }
    else
    {
			/*  
			m_CurrentVolume->GetEventSource()->RemoveObserver(this);
      m_CurrentVolume = NULL;
      for(int i=0; i<m_NumOfChildView; i++)
      ((mafViewSliceLHPBuilder *)m_ChildViewList[i])->UpdateText(0);
			*/
			if(m_AttachCamera)
				m_AttachCamera->SetVme(NULL);
      this->UpdateText(0);
    }
		//CameraUpdate();
    //CameraReset(node);
    //m_Rwi->CameraUpdate();
  }
	else if(node->IsA("mafVMEPolyline")||node->IsA("mafVMESurface")||node->IsA("medVMEPolylineEditor")||node->IsA("mafVMEMesh") || node->IsA("mafVMELandmark") || node->IsA("mafVMELandmarkCloud") /*|| node->IsA("mafVMEMeter")*/)
	{
		this->UpdateSurfacesList(node);
	}
  
	Superclass::VmeShow(node, show);
}
//----------------------------------------------------------------------------
void mafViewSlice::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(vme->IsA("mafVMEPolyline")||vme->IsA("mafVMESurface")||
    vme->IsA("medVMEPolylineEditor")||vme->IsA("mafVMEMesh"))
  {
    this->UpdateSurfacesList(vme);
  }
  Superclass::VmeRemove(vme);
}

//-------------------------------------------------------------------------
void mafViewSlice::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "mafViewSlice" << '\t' << this << std::endl;
  os << indent << "Name" << '\t' << m_Label << std::endl;
  os << std::endl;
  m_Sg->Print(os,1);
}

//-------------------------------------------------------------------------
void mafViewSlice::VolumePositionCorrection(double *point)
//-------------------------------------------------------------------------
{
  if(m_CurrentVolume && m_CurrentVolume->m_Vme)
  {
    mafMatrix *mat = ((mafVME *)m_CurrentVolume->m_Vme)->GetOutput()->GetMatrix();
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
